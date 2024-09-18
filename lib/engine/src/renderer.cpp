#include <cathedral/engine/renderer.hpp>

#include <cathedral/engine/default_resources.hpp>

#include <ien/initializers.hpp>
#include <ien/math_utils.hpp>

namespace cathedral::engine
{
    renderer::renderer(renderer_args args)
        : _args(args)
    {
        const auto surf_size = vkctx().get_surface_size();

        gfx::depthstencil_attachment_args depth_attachment_args;
        depth_attachment_args.vkctx = &vkctx();
        depth_attachment_args.width = surf_size.x;
        depth_attachment_args.height = surf_size.y;

        _depth_attachment = std::make_unique<gfx::depthstencil_attachment>(depth_attachment_args);

        _upload_queue = std::make_unique<upload_queue>(vkctx(), 128 * 1024 * 1024);

        _frame_fence = vkctx().create_signaled_fence();
        _render_ready_semaphore = vkctx().create_default_semaphore();
        _present_ready_semaphore = vkctx().create_default_semaphore();

        _render_cmdbuff = vkctx().create_primary_commandbuffer();

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

    gfx::shader renderer::create_vertex_shader(std::string_view source) const
    {
        gfx::shader_args args;
        args.type = gfx::shader_type::VERTEX;
        args.source = source;

        gfx::shader result(args);
        result.compile();

        return result;
    }

    gfx::shader renderer::create_fragment_shader(std::string_view source) const
    {
        gfx::shader_args args;
        args.type = gfx::shader_type::FRAGMENT;
        args.source = source;

        gfx::shader result(args);
        result.compile();

        return result;
    }

    std::shared_ptr<texture> renderer::create_color_texture(
        const ien::image& img,
        uint32_t mip_levels,
        vk::Filter min_filter,
        vk::Filter mag_filter,
        ien::resize_filter mipgen_filter,
        vk::SamplerAddressMode address_mode,
        uint32_t anisotropy) const
    {
        texture_args args;
        args.image_aspect_flags = vk::ImageAspectFlagBits::eColor;
        args.pimage = &img;
        args.mipgen_filter = mipgen_filter;
        args.request_mipmap_levels = mip_levels;
        args.sampler_args.min_filter = min_filter;
        args.sampler_args.mag_filter = mag_filter;
        args.sampler_args.anisotropy_level = anisotropy;
        args.sampler_args.address_mode = address_mode;
        args.sampler_args.vkctx = &_args.swapchain->vkctx();

        return std::make_shared<texture>(args, *_upload_queue);
    }

    std::shared_ptr<texture> renderer::create_color_texture(
        const std::string& image_path,
        uint32_t mip_levels,
        vk::Filter min_filter,
        vk::Filter mag_filter,
        ien::resize_filter mipgen_filter,
        vk::SamplerAddressMode address_mode,
        uint32_t anisotropy) const
    {
        const ien::image img(image_path);

        texture_args args;
        args.image_aspect_flags = vk::ImageAspectFlagBits::eColor;
        args.pimage = &img;
        args.mipgen_filter = mipgen_filter;
        args.request_mipmap_levels = mip_levels;
        args.sampler_args.min_filter = min_filter;
        args.sampler_args.mag_filter = mag_filter;
        args.sampler_args.address_mode = address_mode;
        args.sampler_args.anisotropy_level = anisotropy;
        args.sampler_args.vkctx = &_args.swapchain->vkctx();
        args.format = texture_format::DXT5_BC3_SRGB;
        args.path = image_path;

        return std::make_shared<texture>(args, *_upload_queue);
    }

    std::shared_ptr<material> renderer::create_material(const material_args& args)
    {
        auto result = std::make_shared<material>(*this, args);
        _materials.emplace(args.name, result);
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
        _render_cmdbuff->reset();
        _render_cmdbuff->begin(vk::CommandBufferBeginInfo{});

        _args.swapchain->transition_undefined_color(_swapchain_image_index, *_render_cmdbuff);

        vk::RenderingAttachmentInfo color_attachment_info;
        color_attachment_info.clearValue.color.float32 = std::array<float, 4>{ 0.0F, 0.0F, 0.0F, 1.0F };
        color_attachment_info.imageLayout = vk::ImageLayout::eColorAttachmentOptimal;
        color_attachment_info.imageView = _args.swapchain->imageview(_swapchain_image_index);
        color_attachment_info.loadOp = vk::AttachmentLoadOp::eClear;
        color_attachment_info.storeOp = vk::AttachmentStoreOp::eStore;
        color_attachment_info.resolveMode = vk::ResolveModeFlagBits::eNone;

        vk::RenderingAttachmentInfo depth_attachment_info;
        depth_attachment_info.clearValue.depthStencil.depth = 1.0F;
        depth_attachment_info.imageLayout = vk::ImageLayout::eDepthStencilAttachmentOptimal;
        depth_attachment_info.imageView = _depth_attachment->depthstencil_imageview();
        depth_attachment_info.loadOp = vk::AttachmentLoadOp::eClear;
        depth_attachment_info.storeOp = vk::AttachmentStoreOp::eDontCare;
        depth_attachment_info.resolveMode = vk::ResolveModeFlagBits::eNone;

        vk::RenderingAttachmentInfo stencil_attachment_info;
        stencil_attachment_info.clearValue.depthStencil.stencil = 0U;
        stencil_attachment_info.imageLayout = vk::ImageLayout::eDepthStencilAttachmentOptimal;
        stencil_attachment_info.imageView = _depth_attachment->depthstencil_imageview();
        stencil_attachment_info.loadOp = vk::AttachmentLoadOp::eClear;
        stencil_attachment_info.storeOp = vk::AttachmentStoreOp::eDontCare;
        stencil_attachment_info.resolveMode = vk::ResolveModeFlagBits::eNone;

        const auto surf_size = vkctx().get_surface_size();

        vk::RenderingInfo rendering_info;
        rendering_info.colorAttachmentCount = 1;
        rendering_info.pColorAttachments = &color_attachment_info;
        rendering_info.layerCount = 1;
        rendering_info.pDepthAttachment = &depth_attachment_info;
        rendering_info.pStencilAttachment = &stencil_attachment_info;
        rendering_info.renderArea.offset = vk::Offset2D(0, 0);
        rendering_info.renderArea.extent = vk::Extent2D(surf_size.x, surf_size.y);
        rendering_info.viewMask = 0;

        _render_cmdbuff->beginRendering(rendering_info);

        vk::Viewport viewport;
        viewport.x = 0;
        viewport.y = 0;
        viewport.width = static_cast<float>(surf_size.x);
        viewport.height = static_cast<float>(surf_size.y);
        viewport.minDepth = 0.0F;
        viewport.maxDepth = 1.0F;

        _render_cmdbuff->setViewport(0, viewport);

        vk::Rect2D scissor;
        scissor.offset = vk::Offset2D(0, 0);
        scissor.extent = vk::Extent2D(surf_size.x, surf_size.y);

        _render_cmdbuff->setScissor(0, scissor);
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
        submit_info.pSignalSemaphores = &*_render_ready_semaphore;
        submit_info.pWaitSemaphores = &image_ready_semaphore;
        submit_info.pWaitDstStageMask = &wait_stage_flags;

        vkctx().graphics_queue().submit(submit_info, _upload_queue->get_fence());

        _upload_queue->notify_submitted();
    }

    void renderer::submit_render_cmdbuff()
    {
        _render_cmdbuff->endRendering();
        _args.swapchain->transition_color_present(_swapchain_image_index, *_render_cmdbuff);
        _render_cmdbuff->end();

        const vk::PipelineStageFlags wait_stage_flags = vk::PipelineStageFlagBits::eAllCommands;

        vk::SubmitInfo submit_info;
        submit_info.commandBufferCount = 1;
        submit_info.pCommandBuffers = &*_render_cmdbuff;
        submit_info.signalSemaphoreCount = 1;
        submit_info.waitSemaphoreCount = 1;
        submit_info.pSignalSemaphores = &*_present_ready_semaphore;
        submit_info.pWaitSemaphores = &*_render_ready_semaphore;
        submit_info.pWaitDstStageMask = &wait_stage_flags;

        vkctx().graphics_queue().submit(submit_info, *_frame_fence);
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
        if (present_result != vk::Result::eSuccess)
        {
            CRITICAL_ERROR("Failure presenting swapchain image");
        }
    }

    void renderer::init_default_texture()
    {
        const auto& default_texture_image = get_default_texture_image();
        _default_texture = create_color_texture(default_texture_image, 8, vk::Filter::eNearest, vk::Filter::eNearest);
    }

    void renderer::init_empty_uniform_buffer()
    {
        gfx::uniform_buffer_args args;
        args.size = 4;
        args.vkctx = &vkctx();

        _empty_uniform_buffer = std::make_unique<gfx::uniform_buffer>(args);
    }
} // namespace cathedral::engine
