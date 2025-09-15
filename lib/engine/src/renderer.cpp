#include <cathedral/engine/renderer.hpp>

#include <cathedral/engine/default_resources.hpp>
#include <cathedral/engine/engine_settings.hpp>
#include <cathedral/gfx/shader_reflection.hpp>
#include <cathedral/memory.hpp>

#include <ien/initializers.hpp>
#include <ien/math_utils.hpp>

#include <magic_enum.hpp>

#include <ranges>
#include <utility>

namespace cathedral::engine
{
    namespace
    {
        uint32_t uid_counter = 0;
    }

    renderer::renderer(renderer_args args)
        : _args(std::move(args))
        , _uid(uid_counter++)
    {
        CRITICAL_CHECK_NOTNULL(_args.engine_settings);

        init_msaa();
        init_main_render_targets();
        init_upload_queue();
        init_semaphores();
        init_commandbuffers();
        init_default_texture();
        init_default_storage_buffer();
        init_empty_uniform_buffer();

        _frame_fence = vkctx().create_signaled_fence();
    }

    void renderer::begin_frame()
    {
        auto wait_fences = get_scratch_vector<vk::Fence>();
        wait_fences.push_back(*_frame_fence);

        if (_upload_queue->fence_needs_waiting())
        {
            wait_fences.push_back(_upload_queue->get_fence());
            _upload_queue->notify_fence_waited();
        }

        if (const vk::Result wait_fence_result = vkctx().device().waitForFences(wait_fences, vk::True, UINT64_MAX);
            wait_fence_result != vk::Result::eSuccess)
        {
            CRITICAL_ERROR("Unable to wait for frame fence!");
        }
        vkctx().device().resetFences(wait_fences);

        const auto surf_size = vkctx().get_surface_size();
        if (std::cmp_not_equal(surf_size.x, _args.swapchain->extent().width) ||
            std::cmp_not_equal(surf_size.y, _args.swapchain->extent().height))
        {
            _args.swapchain->recreate();
            recreate_swapchain_dependent_resources();
        }

        _swapchain_image_index = _args.swapchain->acquire_next_image([this] { reload_depthstencil_attachment(); });

        begin_rendercmd();

        // Process enqueued draw commands (for example, draw calls from outside node tick cycles)
        for (const auto& [domain, cmdlist] : _queued_draw_commands)
        {
            const auto& cmdbuff = render_cmdbuff(domain);
            for (const auto& cmd : cmdlist)
            {
                cmd(cmdbuff);
            }
        }
        _queued_draw_commands.clear();
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
        _args.swapchain->vkctx().device().waitIdle();

        init_main_render_targets();
    }

    std::shared_ptr<texture> renderer::create_color_texture(
        std::string name,
        const ien::image& img,
        const uint32_t mip_levels,
        const vk::Filter min_filter,
        const vk::Filter mag_filter,
        const ien::resize_filter mipgen_filter,
        const vk::SamplerAddressMode address_mode,
        const uint32_t anisotropy)
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

        args.format = [format = img.format()] {
            switch (format)
            {
            case ien::image_format::R:
                return texture_format::R8_SRGB;
            case ien::image_format::RG:
                return texture_format::R8G8_SRGB;
            case ien::image_format::RGB:
                return texture_format::R8G8B8_SRGB;
            case ien::image_format::RGBA:
            default:
                return texture_format::R8G8B8A8_SRGB;
            }
        }();

        auto result = std::make_shared<texture>(args, *_upload_queue);
        _textures.emplace(std::move(name), result);
        return result;
    }

    std::shared_ptr<texture> renderer::create_color_texture(
        std::string name,
        const std::string& image_path,
        const uint32_t mip_levels,
        const vk::Filter min_filter,
        const vk::Filter mag_filter,
        const ien::resize_filter mipgen_filter,
        const vk::SamplerAddressMode address_mode,
        const uint32_t anisotropy)
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

    std::weak_ptr<material> renderer::create_material(material_args args)
    {
        CRITICAL_CHECK(!_materials.contains(args.name), "Attempt to create material with existing name");

        args._internal.msaa_samples = _msaa_samples;
        args._internal.msaa_sample_shading = _msaa_sample_shading;

        auto result = std::make_shared<material>(this, std::move(args));
        _materials.emplace(result->name(), result);
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
        const gfx::image target_image(target_image_args);

        vk::ImageSubresource target_image_subresource;
        target_image_subresource.mipLevel = 0;
        target_image_subresource.arrayLayer = 0;
        target_image_subresource.aspectMask = vk::ImageAspectFlagBits::eColor;

        const vk::SubresourceLayout target_image_subresource_layout =
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

        target_image.transition_layout_suboptimal(
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

        target_image.transition_layout_suboptimal(
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

    std::pair<glm::ivec2, glm::ivec2> renderer::custom_viewport() const
    {
        return _custom_viewport ? *_custom_viewport
                                : std::pair{ glm::ivec2{ 0, 0 }, _args.swapchain->vkctx().get_surface_size() };
    }

    void renderer::set_custom_viewport(const std::optional<std::pair<glm::ivec2, glm::ivec2>>& rect)
    {
        _custom_viewport = rect;
    }

    void renderer::enqueue_draw_command(const render_domain domain, std::function<void(const vk::CommandBuffer& cmdbuff)> call)
    {
        if (!_queued_draw_commands.contains(domain))
        {
            _queued_draw_commands.emplace();
        }
        _queued_draw_commands[domain].push_back(std::move(call));
    }

    void renderer::reload_depthstencil_attachment() const
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

        const bool has_vp = _custom_viewport.has_value();

        const auto x = has_vp ? static_cast<float>(_custom_viewport->first.x) : 0.0F;
        const auto y = has_vp ? static_cast<float>(_custom_viewport->first.y) : 0.0F;
        const auto w = has_vp ? static_cast<float>(_custom_viewport->second.x - _custom_viewport->first.x) : surf_size.x;
        const auto h = has_vp ? static_cast<float>(_custom_viewport->second.y - _custom_viewport->first.y) : surf_size.y;

        vk::Viewport viewport;
        viewport.x = x;
        viewport.y = y;
        viewport.width = w;
        viewport.height = h;
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
        constexpr vk::PipelineStageFlags WAIT_STAGE_FLAGS = vk::PipelineStageFlagBits::eAllCommands;

        const vk::CommandBuffer cmdbuff = { _upload_queue->get_cmdbuff() };

        vk::SubmitInfo submit_info;
        submit_info.commandBufferCount = static_cast<uint32_t>(1);
        submit_info.pCommandBuffers = &cmdbuff;
        submit_info.signalSemaphoreCount = 1;
        submit_info.waitSemaphoreCount = 1;
        submit_info.pSignalSemaphores = &*_render_opaque_ready_semaphore;
        submit_info.pWaitSemaphores = &image_ready_semaphore;
        submit_info.pWaitDstStageMask = &WAIT_STAGE_FLAGS;

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

        constexpr vk::PipelineStageFlags WAIT_STAGE_FLAGS = vk::PipelineStageFlagBits::eAllCommands;

        vk::SubmitInfo submit_opaque_info;
        submit_opaque_info.commandBufferCount = 1;
        submit_opaque_info.pCommandBuffers = &*_render_cmdbuff_opaque;
        submit_opaque_info.signalSemaphoreCount = 1;
        submit_opaque_info.waitSemaphoreCount = 1;
        submit_opaque_info.pSignalSemaphores = &*_render_transparent_ready_semaphore;
        submit_opaque_info.pWaitSemaphores = &*_render_opaque_ready_semaphore;
        submit_opaque_info.pWaitDstStageMask = &WAIT_STAGE_FLAGS;

        vk::SubmitInfo submit_transparent_info;
        submit_transparent_info.commandBufferCount = 1;
        submit_transparent_info.pCommandBuffers = &*_render_cmdbuff_transparent;
        submit_transparent_info.signalSemaphoreCount = 1;
        submit_transparent_info.waitSemaphoreCount = 1;
        submit_transparent_info.pSignalSemaphores = &*_render_overlay_ready_semaphore;
        submit_transparent_info.pWaitSemaphores = &*_render_transparent_ready_semaphore;
        submit_transparent_info.pWaitDstStageMask = &WAIT_STAGE_FLAGS;

        vk::SubmitInfo submit_overlay_info;
        submit_overlay_info.commandBufferCount = 1;
        submit_overlay_info.pCommandBuffers = &*_render_cmdbuff_overlay;
        submit_overlay_info.signalSemaphoreCount = 1;
        submit_overlay_info.waitSemaphoreCount = 1;
        submit_overlay_info.pSignalSemaphores = &*_present_ready_semaphore[_swapchain_image_index];
        submit_overlay_info.pWaitSemaphores = &*_render_overlay_ready_semaphore;
        submit_overlay_info.pWaitDstStageMask = &WAIT_STAGE_FLAGS;

        vkctx().graphics_queue().submit({ submit_opaque_info, submit_transparent_info, submit_overlay_info }, *_frame_fence);
    }

    void renderer::submit_present()
    {
        const vk::SwapchainKHR swapchain = _args.swapchain->get();

        vk::PresentInfoKHR present_info;
        present_info.pImageIndices = &_swapchain_image_index;
        present_info.pSwapchains = &swapchain;
        present_info.swapchainCount = 1;
        present_info.waitSemaphoreCount = 1;
        present_info.pWaitSemaphores = &*_present_ready_semaphore[_swapchain_image_index];
        present_info.pResults = nullptr;

        try
        {
            switch (const vk::Result present_result = vkctx().graphics_queue().presentKHR(present_info))
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
        catch ([[maybe_unused]] const vk::OutOfDateKHRError& err)
        {
            _args.swapchain->recreate();
            recreate_swapchain_dependent_resources();
        }
    }

    void renderer::init_default_texture()
    {
        const auto& default_texture_image = get_default_texture_image();
        _default_texture =
            create_color_texture(DEFAULT_TEXTURE_NAME, default_texture_image, 8, vk::Filter::eNearest, vk::Filter::eNearest);
    }

    void renderer::init_default_storage_buffer()
    {
        gfx::storage_buffer_args args;
        args.size = 4;
        args.vkctx = &vkctx();

        _default_storage_buffer = std::make_shared<gfx::storage_buffer>(std::move(args));
    }

    void renderer::init_empty_uniform_buffer()
    {
        gfx::uniform_buffer_args args;
        args.size = 4;
        args.vkctx = &vkctx();

        _empty_uniform_buffer = std::make_unique<gfx::uniform_buffer>(args);
    }

    void renderer::init_main_render_targets()
    {
        if (_main_render_target_image)
        {
            _main_render_target_image = {};
            _main_render_target_imageview = {};
        }

        const auto surf_size = vkctx().get_surface_size();

        if (_msaa_samples != vk::SampleCountFlagBits::e1)
        {
            const auto main_rt_size = surf_size * static_cast<int>(vk::SampleCountFlagBits::e1);

            gfx::image_args main_rt_image_args = {};
            main_rt_image_args.allow_host_memory_mapping = false;
            main_rt_image_args.aspect_flags = vk::ImageAspectFlagBits::eColor;
            main_rt_image_args.compressed = false;
            main_rt_image_args.format = _args.swapchain->swapchain_image_format();
            main_rt_image_args.height = main_rt_size.y;
            main_rt_image_args.mipmap_levels = 1;
            main_rt_image_args.tiling = vk::ImageTiling::eOptimal;
            main_rt_image_args.usage_flags = vk::ImageUsageFlagBits::eColorAttachment |
                                             vk::ImageUsageFlagBits::eTransientAttachment;
            main_rt_image_args.vkctx = &vkctx();
            main_rt_image_args.width = main_rt_size.x;
            main_rt_image_args.msaa_samples = _msaa_samples;
            _main_render_target_image = std::make_unique<gfx::image>(std::move(main_rt_image_args));

            vk::ImageViewCreateInfo main_rt_imageview_info = {};
            main_rt_imageview_info.format = _args.swapchain->swapchain_image_format();
            main_rt_imageview_info.components.r = vk::ComponentSwizzle::eR;
            main_rt_imageview_info.components.g = vk::ComponentSwizzle::eG;
            main_rt_imageview_info.components.b = vk::ComponentSwizzle::eB;
            main_rt_imageview_info.components.a = vk::ComponentSwizzle::eA;
            main_rt_imageview_info.image = _main_render_target_image->get_image();
            main_rt_imageview_info.subresourceRange.aspectMask = vk::ImageAspectFlagBits::eColor;
            main_rt_imageview_info.subresourceRange.layerCount = 1;
            main_rt_imageview_info.subresourceRange.baseArrayLayer = 0;
            main_rt_imageview_info.subresourceRange.baseMipLevel = 0;
            main_rt_imageview_info.subresourceRange.levelCount = 1;
            main_rt_imageview_info.viewType = vk::ImageViewType::e2D;
            _main_render_target_imageview = vkctx().device().createImageViewUnique(main_rt_imageview_info);

            gfx::depthstencil_attachment_args depth_attachment_args;
            depth_attachment_args.vkctx = &vkctx();
            depth_attachment_args.width = main_rt_size.x;
            depth_attachment_args.height = main_rt_size.y;
            depth_attachment_args.msaa_samples = _msaa_samples;

            _depth_attachment = std::make_unique<gfx::depthstencil_attachment>(depth_attachment_args);
        }
        else
        {
            gfx::depthstencil_attachment_args depth_attachment_args;
            depth_attachment_args.vkctx = &vkctx();
            depth_attachment_args.width = surf_size.x;
            depth_attachment_args.height = surf_size.y;
            depth_attachment_args.msaa_samples = vk::SampleCountFlagBits::e1;

            _depth_attachment = std::make_unique<gfx::depthstencil_attachment>(depth_attachment_args);
        }
    }

    void renderer::init_msaa()
    {
        const auto msaa_evalue = _args.engine_settings->get(engine_setting::MSAA_SAMPLES).as_enum();
        _msaa_samples = [](const setting_enum_value& v) {
            const auto str = v.enum_values.at(v.current_value);
            const auto samples = std::stoi(str);
            return static_cast<vk::SampleCountFlagBits>(samples);
        }(msaa_evalue);

        _msaa_setting_subscription =
            _args.engine_settings->subscribe(engine_setting::MSAA_SAMPLES, [this](const setting_value& value) {
                _msaa_samples = [](const setting_enum_value& v) {
                    const auto str = v.enum_values.at(v.current_value);
                    const auto samples = std::stoi(str);
                    return static_cast<vk::SampleCountFlagBits>(samples);
                }(value.as_enum());

                vkctx().device().waitIdle();
                init_main_render_targets();

                for (const auto& mat : _materials | std::views::values)
                {
                    mat->set_msaa_samples(_msaa_samples);
                }
            });

        _msaa_sample_shading = _args.engine_settings->get(engine_setting::MSAA_SAMPLE_SHADING).as_bool();
        _msaa_sample_shading_subscription =
            _args.engine_settings->subscribe(engine_setting::MSAA_SAMPLE_SHADING, [this](const setting_value& value) {
                _msaa_sample_shading = value.as_bool();

                vkctx().device().waitIdle();
                for (const auto& mat : _materials | std::views::values)
                {
                    mat->set_msaa_sample_shading(_msaa_sample_shading);
                }
            });
    }

    void renderer::init_upload_queue()
    {
        const auto upload_queue_size = _args.engine_settings->get(engine_setting::UPLOAD_QUEUE_SIZE_MB);
        _upload_queue = std::make_unique<upload_queue>(vkctx(), upload_queue_size.as_int() * 1024 * 1024);
        log_info(std::format("Initialized upload queue with {}MB of space", upload_queue_size.as_int()));
    }

    void renderer::init_semaphores()
    {
        _render_opaque_ready_semaphore = vkctx().create_default_semaphore();
        _render_transparent_ready_semaphore = vkctx().create_default_semaphore();
        _render_overlay_ready_semaphore = vkctx().create_default_semaphore();
        for (size_t i = 0; i < _args.swapchain->image_count(); ++i)
        {
            _present_ready_semaphore.push_back(vkctx().create_default_semaphore());
        }
    }

    void renderer::init_commandbuffers()
    {
        _render_cmdbuff_opaque = vkctx().create_primary_commandbuffer();
        _render_cmdbuff_transparent = vkctx().create_primary_commandbuffer();
        _render_cmdbuff_overlay = vkctx().create_primary_commandbuffer();
    }

    void renderer::begin_opaque_pass(glm::ivec2 surf_size)
    {
        _render_cmdbuff_opaque->reset();
        _render_cmdbuff_opaque->begin(vk::CommandBufferBeginInfo{});

        _args.swapchain->transition_undefined_color(_swapchain_image_index, *_render_cmdbuff_opaque);

        vk::RenderingAttachmentInfo opaque_pass_color_attachment_info;
        opaque_pass_color_attachment_info.clearValue.color.float32 = std::array{ 0.0F, 0.0F, 0.0F, 1.0F };
        opaque_pass_color_attachment_info.imageLayout = vk::ImageLayout::eColorAttachmentOptimal;
        opaque_pass_color_attachment_info.imageView = (_msaa_samples != vk::SampleCountFlagBits::e1)
                                                          ? *_main_render_target_imageview
                                                          : _args.swapchain->imageview(_swapchain_image_index);
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
        transparent_pass_color_attachment_info.clearValue.color.float32 = std::array{ 0.0F, 0.0F, 0.0F, 1.0F };
        transparent_pass_color_attachment_info.imageLayout = vk::ImageLayout::eColorAttachmentOptimal;
        transparent_pass_color_attachment_info.imageView = (_msaa_samples != vk::SampleCountFlagBits::e1)
                                                               ? *_main_render_target_imageview
                                                               : _args.swapchain->imageview(_swapchain_image_index);
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
        overlay_pass_color_attachment_info.clearValue.color.float32 = std::array{ 0.0F, 0.0F, 0.0F, 1.0F };
        overlay_pass_color_attachment_info.imageLayout = vk::ImageLayout::eColorAttachmentOptimal;
        overlay_pass_color_attachment_info.imageView = (_msaa_samples != vk::SampleCountFlagBits::e1)
                                                           ? *_main_render_target_imageview
                                                           : _args.swapchain->imageview(_swapchain_image_index);
        overlay_pass_color_attachment_info.loadOp = vk::AttachmentLoadOp::eLoad;

        // With MSAA enabled, there is no need to preseve the color attachment after the resolve step,
        // potentially improving bandwidth
        overlay_pass_color_attachment_info.storeOp = (_msaa_samples != vk::SampleCountFlagBits::e1)
                                                         ? vk::AttachmentStoreOp::eDontCare
                                                         : vk::AttachmentStoreOp::eStore;

        overlay_pass_color_attachment_info.resolveMode = (_msaa_samples != vk::SampleCountFlagBits::e1)
                                                             ? vk::ResolveModeFlagBits::eAverage
                                                             : vk::ResolveModeFlagBits::eNone;
        overlay_pass_color_attachment_info.resolveImageView =
            (_msaa_samples != vk::SampleCountFlagBits::e1) ? _args.swapchain->imageview(_swapchain_image_index) : nullptr;
        overlay_pass_color_attachment_info.resolveImageLayout = vk::ImageLayout::eColorAttachmentOptimal;

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
