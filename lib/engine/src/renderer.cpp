#include <cathedral/engine/renderer.hpp>

#include <cathedral/engine/default_resources.hpp>
#include <cathedral/engine/shader_preprocess.hpp>

#include <cathedral/gfx/shader_reflection.hpp>

#include <ien/initializers.hpp>
#include <ien/math_utils.hpp>

#include <magic_enum.hpp>

namespace cathedral::engine
{
    renderer::renderer(renderer_args args)
        : _args(std::move(args))
    {
        const auto surf_size = vkctx().get_surface_size();

        gfx::depthstencil_attachment_args depth_attachment_args;
        depth_attachment_args.vkctx = &vkctx();
        depth_attachment_args.width = surf_size.x;
        depth_attachment_args.height = surf_size.y;

        _depth_attachment = std::make_unique<gfx::depthstencil_attachment>(depth_attachment_args);

        _upload_queue = std::make_unique<upload_queue>(vkctx(), 128 * 1024 * 1024);

        _frame_fence = vkctx().create_signaled_fence();
        _render_opaque_ready_semaphore = vkctx().create_default_semaphore();
        _render_transparent_ready_semaphore = vkctx().create_default_semaphore();
        _render_overlay_ready_semaphore = vkctx().create_default_semaphore();
        _present_ready_semaphore = vkctx().create_default_semaphore();

        _render_cmdbuff_opaque = vkctx().create_primary_commandbuffer();
        _render_cmdbuff_transparent = vkctx().create_primary_commandbuffer();
        _render_cmdbuff_overlay = vkctx().create_primary_commandbuffer();

        init_default_texture();
        init_empty_uniform_buffer();
    }

    void renderer::begin_frame()
    {
        std::vector<vk::Fence> wait_fences = { *_frame_fence };
        if (_upload_queue->fence_needs_waiting())
        {
            wait_fences.push_back(_upload_queue->get_fence());
            _upload_queue->notify_fence_waited();
        }

        const vk::Result wait_fence_result = vkctx().device().waitForFences(wait_fences, vk::True, UINT64_MAX);
        if (wait_fence_result != vk::Result::eSuccess)
        {
            CRITICAL_ERROR("Unable to wait for frame fence!");
        }
        vkctx().device().resetFences(wait_fences);

        auto surf_size = vkctx().get_surface_size();
        while (surf_size.x != _args.swapchain->extent().width || surf_size.y != _args.swapchain->extent().height)
        {
            _args.swapchain->recreate();
            surf_size = vkctx().get_surface_size();
        }

        _swapchain_image_index = _args.swapchain->acquire_next_image([this] { reload_depthstencil_attachment(); });

        begin_rendercmd();
    }

    void renderer::end_frame()
    {
        submit_prerender_cmdbuffs();
        submit_render_cmdbuff();
        submit_present();

        ++_frame_count;
    }

    void renderer::recreate_swapchain_dependent_resources()
    {
        const auto surf_size = vkctx().get_surface_size();

        gfx::depthstencil_attachment_args args;
        args.vkctx = &vkctx();
        args.width = surf_size.x;
        args.height = surf_size.y;

        _depth_attachment->reload(args);
    }

    std::shared_ptr<texture> renderer::create_color_texture(
        std::string name,
        const ien::image& img,
        uint32_t mip_levels,
        vk::Filter min_filter,
        vk::Filter mag_filter,
        ien::resize_filter mipgen_filter,
        vk::SamplerAddressMode address_mode,
        uint32_t anisotropy)
    {
        CRITICAL_CHECK(!name.empty(), "Texture name cannot be empty");
        CRITICAL_CHECK(!_textures.contains(name), "Attempt to create texture with existing name");

        texture_args_from_path args;
        args.name = name;
        args.image_aspect_flags = vk::ImageAspectFlagBits::eColor;
        args.pimage = &img;
        args.mipgen_filter = mipgen_filter;
        args.request_mipmap_levels = mip_levels;
        args.sampler_info.min_filter = min_filter;
        args.sampler_info.mag_filter = mag_filter;
        args.sampler_info.anisotropy_level = anisotropy;
        args.sampler_info.address_mode = address_mode;

        auto result = std::make_shared<texture>(args, *_upload_queue);
        _textures.emplace(std::move(name), result);
        return result;
    }

    std::shared_ptr<texture> renderer::create_color_texture(
        std::string name,
        const std::string& image_path,
        uint32_t mip_levels,
        vk::Filter min_filter,
        vk::Filter mag_filter,
        ien::resize_filter mipgen_filter,
        vk::SamplerAddressMode address_mode,
        uint32_t anisotropy)
    {
        CRITICAL_CHECK(!name.empty(), "Texture name cannot be empty");
        CRITICAL_CHECK(!_textures.contains(name), "Attempt to create texture with existing name");

        const ien::image img(image_path);

        texture_args_from_path args;
        args.name = name;
        args.image_aspect_flags = vk::ImageAspectFlagBits::eColor;
        args.pimage = &img;
        args.mipgen_filter = mipgen_filter;
        args.request_mipmap_levels = mip_levels;
        args.sampler_info.min_filter = min_filter;
        args.sampler_info.mag_filter = mag_filter;
        args.sampler_info.address_mode = address_mode;
        args.sampler_info.anisotropy_level = anisotropy;
        args.format = texture_format::DXT5_BC3_SRGB;
        args.path = image_path;

        auto result = std::make_shared<texture>(args, *_upload_queue);
        _textures.emplace(std::move(name), result);
        return result;
    }

    std::shared_ptr<texture> renderer::create_color_texture_from_data(const texture_args_from_data& args)
    {
        auto result = std::make_shared<texture>(args, *_upload_queue);
        _textures.emplace(args.name, result);
        return result;
    }

    std::weak_ptr<material> renderer::create_material(const material_args& args)
    {
        CRITICAL_CHECK(!_materials.contains(args.name), "Attempt to create material with existing name");

        auto result = std::make_shared<material>(this, args);
        _materials.emplace(args.name, result);
        return result;
    }

    ien::image renderer::capture_screenshot() const
    {
        auto* swapchain = _args.swapchain;
        const auto& vkctx = swapchain->vkctx();
        vkctx.device().waitIdle();

        const auto surf_size = vkctx.get_surface_size();

        const auto& swapchain_image = swapchain->image(_swapchain_image_index);

        gfx::image_args target_image_args;
        target_image_args.aspect_flags = vk::ImageAspectFlagBits::eColor;
        target_image_args.compressed = false;
        target_image_args.format = vk::Format::eR8G8B8A8Srgb;
        target_image_args.width = surf_size.x;
        target_image_args.height = surf_size.y;
        target_image_args.mipmap_levels = 1;
        target_image_args.vkctx = &swapchain->vkctx();
        target_image_args.tiling = vk::ImageTiling::eLinear;
        target_image_args.usage_flags = vk::ImageUsageFlagBits::eTransferDst;
        target_image_args.allow_host_memory_mapping = true;
        gfx::image target_image(target_image_args);

        vk::ImageSubresource target_image_subresource;
        target_image_subresource.mipLevel = 0;
        target_image_subresource.arrayLayer = 0;
        target_image_subresource.aspectMask = vk::ImageAspectFlagBits::eColor;

        vk::SubresourceLayout target_image_subresource_layout =
            vkctx.device().getImageSubresourceLayout(target_image.get_image(), target_image_subresource);

        auto cmdbuff = swapchain->vkctx().create_primary_commandbuffer();
        cmdbuff->begin(vk::CommandBufferBeginInfo{});

        // Transition swapchain image to TransferSrcOptimal
        {
            vk::ImageMemoryBarrier2 barrier;
            barrier.image = swapchain_image;
            barrier.oldLayout = vk::ImageLayout::ePresentSrcKHR;
            barrier.newLayout = vk::ImageLayout::eTransferSrcOptimal;
            barrier.srcAccessMask = vk::AccessFlagBits2::eMemoryRead | vk::AccessFlagBits2::eMemoryWrite;
            barrier.srcStageMask = vk::PipelineStageFlagBits2::eAllCommands;
            barrier.srcQueueFamilyIndex = vkctx.graphics_queue_family_index();
            barrier.dstAccessMask = vk::AccessFlagBits2::eMemoryRead | vk::AccessFlagBits2::eMemoryWrite;
            barrier.dstStageMask = vk::PipelineStageFlagBits2::eAllCommands;
            barrier.dstQueueFamilyIndex = vkctx.graphics_queue_family_index();
            barrier.subresourceRange.aspectMask = vk::ImageAspectFlagBits::eColor;
            barrier.subresourceRange.baseArrayLayer = 0;
            barrier.subresourceRange.baseMipLevel = 0;
            barrier.subresourceRange.layerCount = 1;
            barrier.subresourceRange.levelCount = 1;

            vk::DependencyInfo depinfo;
            depinfo.imageMemoryBarrierCount = 1;
            depinfo.pImageMemoryBarriers = &barrier;

            cmdbuff->pipelineBarrier2(depinfo);
        }

        target_image.transition_layout(
            vk::ImageLayout::eUndefined,
            vk::ImageLayout::eTransferDstOptimal,
            *cmdbuff,
            vk::ImageAspectFlagBits::eColor,
            0,
            1);

        vk::ImageBlit blit;
        blit.srcOffsets[0] = vk::Offset3D{ .x = 0, .y = 0, .z = 0 };
        blit.srcOffsets[1] = vk::Offset3D{ .x = surf_size.x, .y = surf_size.y, .z = 1 };
        blit.dstOffsets[0] = vk::Offset3D{ .x = 0, .y = 0, .z = 0 };
        blit.dstOffsets[1] = vk::Offset3D{ .x = static_cast<int32_t>(target_image.width()),
                                           .y = static_cast<int32_t>(target_image.height()),
                                           .z = 1 };
        blit.srcSubresource.aspectMask = vk::ImageAspectFlagBits::eColor;
        blit.dstSubresource.aspectMask = vk::ImageAspectFlagBits::eColor;
        blit.srcSubresource.layerCount = 1;
        blit.dstSubresource.layerCount = 1;
        blit.srcSubresource.mipLevel = 0;
        blit.dstSubresource.mipLevel = 0;

        cmdbuff->blitImage(
            swapchain_image,
            vk::ImageLayout::eTransferSrcOptimal,
            target_image.get_image(),
            vk::ImageLayout::eTransferDstOptimal,
            blit,
            vk::Filter::eLinear);

        target_image.transition_layout(
            vk::ImageLayout::eTransferDstOptimal,
            vk::ImageLayout::eGeneral,
            *cmdbuff,
            vk::ImageAspectFlagBits::eColor,
            0,
            1);

        // Transition swapchain image to PresentSrcKHR
        {
            vk::ImageMemoryBarrier2 barrier;
            barrier.image = swapchain_image;
            barrier.oldLayout = vk::ImageLayout::eTransferSrcOptimal;
            barrier.newLayout = vk::ImageLayout::ePresentSrcKHR;
            barrier.srcAccessMask = vk::AccessFlagBits2::eMemoryRead | vk::AccessFlagBits2::eMemoryWrite;
            barrier.srcStageMask = vk::PipelineStageFlagBits2::eAllCommands;
            barrier.srcQueueFamilyIndex = vkctx.graphics_queue_family_index();
            barrier.dstAccessMask = vk::AccessFlagBits2::eMemoryRead | vk::AccessFlagBits2::eMemoryWrite;
            barrier.dstStageMask = vk::PipelineStageFlagBits2::eAllCommands;
            barrier.dstQueueFamilyIndex = vkctx.graphics_queue_family_index();
            barrier.subresourceRange.aspectMask = vk::ImageAspectFlagBits::eColor;
            barrier.subresourceRange.baseArrayLayer = 0;
            barrier.subresourceRange.baseMipLevel = 0;
            barrier.subresourceRange.layerCount = 1;
            barrier.subresourceRange.levelCount = 1;

            vk::DependencyInfo depinfo;
            depinfo.imageMemoryBarrierCount = 1;
            depinfo.pImageMemoryBarriers = &barrier;

            cmdbuff->pipelineBarrier2(depinfo);
        }

        cmdbuff->end();

        vkctx.submit_commandbuffer_sync(*cmdbuff);

        ien::image result(target_image.width(), target_image.height());

        void* mapped_memory = nullptr;
        vmaMapMemory(vkctx.allocator(), target_image.allocation(), &mapped_memory);

        // Copy data row by row, since vulkan image memory might have a non obvious row byte size
        const auto dst_row_size = target_image.width() * 4;
        for (size_t row = 0; row < target_image.height(); ++row)
        {
            const size_t source_data_offset = row * target_image_subresource_layout.rowPitch;
            const size_t destination_data_offset = row * dst_row_size;
            std::memcpy(
                result.data() + destination_data_offset,
                static_cast<const uint8_t*>(mapped_memory) + source_data_offset,
                dst_row_size);
        }
        vmaUnmapMemory(vkctx.allocator(), target_image.allocation());

        return result;
    }

    void renderer::reload_depthstencil_attachment()
    {
        const auto surf_size = vkctx().get_surface_size();
        gfx::depthstencil_attachment_args depth_attachment_args;
        depth_attachment_args.vkctx = &vkctx();
        depth_attachment_args.width = surf_size.x;
        depth_attachment_args.height = surf_size.y;
        _depth_attachment->reload(depth_attachment_args);
    }

    void renderer::begin_rendercmd()
    {
        const auto surf_size = vkctx().get_surface_size();

        begin_opaque_pass(surf_size);
        begin_transparent_pass(surf_size);
        begin_overlay_pass(surf_size);

        vk::Viewport viewport;
        viewport.x = 0;
        viewport.y = 0;
        viewport.width = static_cast<float>(surf_size.x);
        viewport.height = static_cast<float>(surf_size.y);
        viewport.minDepth = 0.0F;
        viewport.maxDepth = 1.0F;

        _render_cmdbuff_opaque->setViewport(0, viewport);
        _render_cmdbuff_transparent->setViewport(0, viewport);
        _render_cmdbuff_overlay->setViewport(0, viewport);

        vk::Rect2D scissor;
        scissor.offset = vk::Offset2D(0, 0);
        scissor.extent = vk::Extent2D(surf_size.x, surf_size.y);

        _render_cmdbuff_opaque->setScissor(0, scissor);
        _render_cmdbuff_transparent->setScissor(0, scissor);
        _render_cmdbuff_overlay->setScissor(0, scissor);
    }

    void renderer::submit_prerender_cmdbuffs()
    {
        _upload_queue->prepare_to_submit();

        const auto image_ready_semaphore = _args.swapchain->image_ready_semaphore();
        const vk::PipelineStageFlags wait_stage_flags = vk::PipelineStageFlagBits::eAllCommands;

        const std::vector<vk::CommandBuffer> cmdbuffs = { _upload_queue->get_cmdbuff() };

        vk::SubmitInfo submit_info;
        submit_info.commandBufferCount = static_cast<uint32_t>(cmdbuffs.size());
        submit_info.pCommandBuffers = cmdbuffs.data();
        submit_info.signalSemaphoreCount = 1;
        submit_info.waitSemaphoreCount = 1;
        submit_info.pSignalSemaphores = &*_render_opaque_ready_semaphore;
        submit_info.pWaitSemaphores = &image_ready_semaphore;
        submit_info.pWaitDstStageMask = &wait_stage_flags;

        vkctx().graphics_queue().submit(submit_info, _upload_queue->get_fence());

        _upload_queue->notify_submitted();
    }

    void renderer::submit_render_cmdbuff()
    {
        _render_cmdbuff_opaque->endRendering();
        _render_cmdbuff_transparent->endRendering();
        _render_cmdbuff_overlay->endRendering();

        _args.swapchain->transition_color_present(_swapchain_image_index, *_render_cmdbuff_overlay);

        _render_cmdbuff_opaque->end();
        _render_cmdbuff_transparent->end();
        _render_cmdbuff_overlay->end();

        const vk::PipelineStageFlags wait_stage_flags = vk::PipelineStageFlagBits::eAllCommands;

        vk::SubmitInfo submit_opaque_info;
        submit_opaque_info.commandBufferCount = 1;
        submit_opaque_info.pCommandBuffers = &*_render_cmdbuff_opaque;
        submit_opaque_info.signalSemaphoreCount = 1;
        submit_opaque_info.waitSemaphoreCount = 1;
        submit_opaque_info.pSignalSemaphores = &*_render_transparent_ready_semaphore;
        submit_opaque_info.pWaitSemaphores = &*_render_opaque_ready_semaphore;
        submit_opaque_info.pWaitDstStageMask = &wait_stage_flags;

        vk::SubmitInfo submit_transparent_info;
        submit_transparent_info.commandBufferCount = 1;
        submit_transparent_info.pCommandBuffers = &*_render_cmdbuff_transparent;
        submit_transparent_info.signalSemaphoreCount = 1;
        submit_transparent_info.waitSemaphoreCount = 1;
        submit_transparent_info.pSignalSemaphores = &*_render_overlay_ready_semaphore;
        submit_transparent_info.pWaitSemaphores = &*_render_transparent_ready_semaphore;
        submit_transparent_info.pWaitDstStageMask = &wait_stage_flags;

        vk::SubmitInfo submit_overlay_info;
        submit_overlay_info.commandBufferCount = 1;
        submit_overlay_info.pCommandBuffers = &*_render_cmdbuff_overlay;
        submit_overlay_info.signalSemaphoreCount = 1;
        submit_overlay_info.waitSemaphoreCount = 1;
        submit_overlay_info.pSignalSemaphores = &*_present_ready_semaphore;
        submit_overlay_info.pWaitSemaphores = &*_render_overlay_ready_semaphore;
        submit_overlay_info.pWaitDstStageMask = &wait_stage_flags;

        vkctx().graphics_queue().submit({ submit_opaque_info, submit_transparent_info, submit_overlay_info }, *_frame_fence);
    }

    void renderer::submit_present()
    {
        vk::SwapchainKHR swapchain = _args.swapchain->get();

        vk::PresentInfoKHR present_info;
        present_info.pImageIndices = &_swapchain_image_index;
        present_info.pSwapchains = &swapchain;
        present_info.swapchainCount = 1;
        present_info.waitSemaphoreCount = 1;
        present_info.pWaitSemaphores = &*_present_ready_semaphore;
        present_info.pResults = nullptr;

        const vk::Result present_result = vkctx().graphics_queue().presentKHR(present_info);
        switch (present_result)
        {
        case vk::Result::eSuccess:
            break;
        case vk::Result::eErrorOutOfDateKHR:
        case vk::Result::eSuboptimalKHR:
            _args.swapchain->recreate();
            recreate_swapchain_dependent_resources();
            break;
        default:
            CRITICAL_ERROR(std::format("Unhandled present result: {}", magic_enum::enum_name(present_result)));
        }
    }

    void renderer::init_default_texture()
    {
        const auto& default_texture_image = get_default_texture_image();
        _default_texture =
            create_color_texture(DEFAULT_TEXTURE_NAME, default_texture_image, 8, vk::Filter::eNearest, vk::Filter::eNearest);
    }

    void renderer::init_empty_uniform_buffer()
    {
        gfx::uniform_buffer_args args;
        args.size = 4;
        args.vkctx = &vkctx();

        _empty_uniform_buffer = std::make_unique<gfx::uniform_buffer>(args);
    }

    void renderer::begin_opaque_pass(glm::ivec2 surf_size)
    {
        _render_cmdbuff_opaque->reset();
        _render_cmdbuff_opaque->begin(vk::CommandBufferBeginInfo{});

        _args.swapchain->transition_undefined_color(_swapchain_image_index, *_render_cmdbuff_opaque);

        vk::RenderingAttachmentInfo opaque_pass_color_attachment_info;
        opaque_pass_color_attachment_info.clearValue.color.float32 = std::array<float, 4>{ 0.0F, 0.0F, 0.0F, 1.0F };
        opaque_pass_color_attachment_info.imageLayout = vk::ImageLayout::eColorAttachmentOptimal;
        opaque_pass_color_attachment_info.imageView = _args.swapchain->imageview(_swapchain_image_index);
        opaque_pass_color_attachment_info.loadOp = vk::AttachmentLoadOp::eClear;
        opaque_pass_color_attachment_info.storeOp = vk::AttachmentStoreOp::eStore;
        opaque_pass_color_attachment_info.resolveMode = vk::ResolveModeFlagBits::eNone;

        vk::RenderingAttachmentInfo opaque_pass_depth_attachment_info;
        opaque_pass_depth_attachment_info.clearValue.depthStencil.depth = 1.0F;
        opaque_pass_depth_attachment_info.imageLayout = vk::ImageLayout::eDepthStencilAttachmentOptimal;
        opaque_pass_depth_attachment_info.imageView = _depth_attachment->depthstencil_imageview();
        opaque_pass_depth_attachment_info.loadOp = vk::AttachmentLoadOp::eClear;
        opaque_pass_depth_attachment_info.storeOp = vk::AttachmentStoreOp::eStore;
        opaque_pass_depth_attachment_info.resolveMode = vk::ResolveModeFlagBits::eNone;

        vk::RenderingAttachmentInfo opaque_pass_stencil_attachment_info;
        opaque_pass_stencil_attachment_info.clearValue.depthStencil.stencil = 0U;
        opaque_pass_stencil_attachment_info.imageLayout = vk::ImageLayout::eDepthStencilAttachmentOptimal;
        opaque_pass_stencil_attachment_info.imageView = _depth_attachment->depthstencil_imageview();
        opaque_pass_stencil_attachment_info.loadOp = vk::AttachmentLoadOp::eClear;
        opaque_pass_stencil_attachment_info.storeOp = vk::AttachmentStoreOp::eStore;
        opaque_pass_stencil_attachment_info.resolveMode = vk::ResolveModeFlagBits::eNone;

        vk::RenderingInfo opaque_pass_rendering_info;
        opaque_pass_rendering_info.colorAttachmentCount = 1;
        opaque_pass_rendering_info.pColorAttachments = &opaque_pass_color_attachment_info;
        opaque_pass_rendering_info.layerCount = 1;
        opaque_pass_rendering_info.pDepthAttachment = &opaque_pass_depth_attachment_info;
        opaque_pass_rendering_info.pStencilAttachment = &opaque_pass_stencil_attachment_info;
        opaque_pass_rendering_info.renderArea.offset = vk::Offset2D(0, 0);
        opaque_pass_rendering_info.renderArea.extent = vk::Extent2D(surf_size.x, surf_size.y);
        opaque_pass_rendering_info.viewMask = 0;

        _render_cmdbuff_opaque->beginRendering(opaque_pass_rendering_info);
    }

    void renderer::begin_transparent_pass(glm::ivec2 surf_size)
    {
        _render_cmdbuff_transparent->reset();
        _render_cmdbuff_transparent->begin(vk::CommandBufferBeginInfo{});

        vk::RenderingAttachmentInfo transparent_pass_color_attachment_info;
        transparent_pass_color_attachment_info.clearValue.color.float32 = std::array<float, 4>{ 0.0F, 0.0F, 0.0F, 1.0F };
        transparent_pass_color_attachment_info.imageLayout = vk::ImageLayout::eColorAttachmentOptimal;
        transparent_pass_color_attachment_info.imageView = _args.swapchain->imageview(_swapchain_image_index);
        transparent_pass_color_attachment_info.loadOp = vk::AttachmentLoadOp::eLoad;
        transparent_pass_color_attachment_info.storeOp = vk::AttachmentStoreOp::eStore;
        transparent_pass_color_attachment_info.resolveMode = vk::ResolveModeFlagBits::eNone;

        vk::RenderingAttachmentInfo transparent_pass_depth_attachment_info;
        transparent_pass_depth_attachment_info.clearValue.depthStencil.depth = 1.0F;
        transparent_pass_depth_attachment_info.imageLayout = vk::ImageLayout::eDepthStencilAttachmentOptimal;
        transparent_pass_depth_attachment_info.imageView = _depth_attachment->depthstencil_imageview();
        transparent_pass_depth_attachment_info.loadOp = vk::AttachmentLoadOp::eLoad;
        transparent_pass_depth_attachment_info.storeOp = vk::AttachmentStoreOp::eDontCare;
        transparent_pass_depth_attachment_info.resolveMode = vk::ResolveModeFlagBits::eNone;

        vk::RenderingAttachmentInfo transparent_pass_stencil_attachment_info;
        transparent_pass_stencil_attachment_info.clearValue.depthStencil.stencil = 0U;
        transparent_pass_stencil_attachment_info.imageLayout = vk::ImageLayout::eDepthStencilAttachmentOptimal;
        transparent_pass_stencil_attachment_info.imageView = _depth_attachment->depthstencil_imageview();
        transparent_pass_stencil_attachment_info.loadOp = vk::AttachmentLoadOp::eLoad;
        transparent_pass_stencil_attachment_info.storeOp = vk::AttachmentStoreOp::eDontCare;
        transparent_pass_stencil_attachment_info.resolveMode = vk::ResolveModeFlagBits::eNone;

        vk::RenderingInfo transparent_pass_rendering_info;
        transparent_pass_rendering_info.colorAttachmentCount = 1;
        transparent_pass_rendering_info.pColorAttachments = &transparent_pass_color_attachment_info;
        transparent_pass_rendering_info.layerCount = 1;
        transparent_pass_rendering_info.pDepthAttachment = &transparent_pass_depth_attachment_info;
        transparent_pass_rendering_info.pStencilAttachment = &transparent_pass_stencil_attachment_info;
        transparent_pass_rendering_info.renderArea.offset = vk::Offset2D(0, 0);
        transparent_pass_rendering_info.renderArea.extent = vk::Extent2D(surf_size.x, surf_size.y);
        transparent_pass_rendering_info.viewMask = 0;

        _render_cmdbuff_transparent->beginRendering(transparent_pass_rendering_info);
    }

    void renderer::begin_overlay_pass(glm::ivec2 surf_size)
    {
        _render_cmdbuff_overlay->reset();
        _render_cmdbuff_overlay->begin(vk::CommandBufferBeginInfo{});

        vk::RenderingAttachmentInfo overlay_pass_color_attachment_info;
        overlay_pass_color_attachment_info.clearValue.color.float32 = std::array<float, 4>{ 0.0F, 0.0F, 0.0F, 1.0F };
        overlay_pass_color_attachment_info.imageLayout = vk::ImageLayout::eColorAttachmentOptimal;
        overlay_pass_color_attachment_info.imageView = _args.swapchain->imageview(_swapchain_image_index);
        overlay_pass_color_attachment_info.loadOp = vk::AttachmentLoadOp::eLoad;
        overlay_pass_color_attachment_info.storeOp = vk::AttachmentStoreOp::eStore;
        overlay_pass_color_attachment_info.resolveMode = vk::ResolveModeFlagBits::eNone;

        vk::RenderingAttachmentInfo overlay_pass_depth_attachment_info;
        overlay_pass_depth_attachment_info.clearValue.depthStencil.depth = 1.0F;
        overlay_pass_depth_attachment_info.imageLayout = vk::ImageLayout::eDepthStencilAttachmentOptimal;
        overlay_pass_depth_attachment_info.imageView = _depth_attachment->depthstencil_imageview();
        overlay_pass_depth_attachment_info.loadOp = vk::AttachmentLoadOp::eClear;
        overlay_pass_depth_attachment_info.storeOp = vk::AttachmentStoreOp::eDontCare;
        overlay_pass_depth_attachment_info.resolveMode = vk::ResolveModeFlagBits::eNone;

        vk::RenderingAttachmentInfo overlay_pass_stencil_attachment_info;
        overlay_pass_stencil_attachment_info.clearValue.depthStencil.stencil = 0U;
        overlay_pass_stencil_attachment_info.imageLayout = vk::ImageLayout::eDepthStencilAttachmentOptimal;
        overlay_pass_stencil_attachment_info.imageView = _depth_attachment->depthstencil_imageview();
        overlay_pass_stencil_attachment_info.loadOp = vk::AttachmentLoadOp::eClear;
        overlay_pass_stencil_attachment_info.storeOp = vk::AttachmentStoreOp::eDontCare;
        overlay_pass_stencil_attachment_info.resolveMode = vk::ResolveModeFlagBits::eNone;

        vk::RenderingInfo overlay_pass_rendering_info;
        overlay_pass_rendering_info.colorAttachmentCount = 1;
        overlay_pass_rendering_info.pColorAttachments = &overlay_pass_color_attachment_info;
        overlay_pass_rendering_info.layerCount = 1;
        overlay_pass_rendering_info.pDepthAttachment = &overlay_pass_depth_attachment_info;
        overlay_pass_rendering_info.pStencilAttachment = &overlay_pass_stencil_attachment_info;
        overlay_pass_rendering_info.renderArea.offset = vk::Offset2D(0, 0);
        overlay_pass_rendering_info.renderArea.extent = vk::Extent2D(surf_size.x, surf_size.y);
        overlay_pass_rendering_info.viewMask = 0;

        _render_cmdbuff_overlay->beginRendering(overlay_pass_rendering_info);
    }
} // namespace cathedral::engine
