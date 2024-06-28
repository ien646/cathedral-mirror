#include <cathedral/engine/renderer.hpp>

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
    }

    void renderer::begin_frame()
    {
        const vk::Result wait_fence_result = vkctx().device().waitForFences(*_frame_fence, true, UINT64_MAX);
        if (wait_fence_result != vk::Result::eSuccess)
        {
            die("Unable to wait for frame fence!");
        }
        vkctx().device().resetFences(*_frame_fence);

        _swapchain_image_index = _args.swapchain->acquire_next_image([&] { reload_depthstencil_attachment(); });

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

    gfx::shader renderer::create_vertex_shader(const std::string& source) const
    {
        gfx::shader_args args;
        args.type = gfx::shader_type::VERTEX;
        args.store_spirv = is_debug_build();
        args.source = source;
        args.vkctx = &vkctx();

        return gfx::shader(args);
    }

    gfx::shader renderer::create_fragment_shader(const std::string& source) const
    {
        gfx::shader_args args;
        args.type = gfx::shader_type::FRAGMENT;
        args.store_spirv = is_debug_build();
        args.source = source;
        args.vkctx = &vkctx();

        return gfx::shader(args);
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

        const float segment_part0 = std::abs(ien::remap(_frame_count % 213, 0, 117, -1.0f, 1.0f));
        const float segment_part1 = std::abs(ien::remap(_frame_count % 379, 0, 221, -1.0f, 1.0f));
        const float segment_part2 = std::abs(ien::remap(_frame_count % 555, 0, 333, -1.0f, 1.0f));

        vk::RenderingAttachmentInfo color_attachment_info;
        color_attachment_info.clearValue.color
            .float32 = std::array<float, 4>{ segment_part0, segment_part1, segment_part2, 1.0f };
        color_attachment_info.imageLayout = vk::ImageLayout::eColorAttachmentOptimal;
        color_attachment_info.imageView = _args.swapchain->imageview(_swapchain_image_index);
        color_attachment_info.loadOp = vk::AttachmentLoadOp::eClear;
        color_attachment_info.storeOp = vk::AttachmentStoreOp::eStore;
        color_attachment_info.resolveMode = vk::ResolveModeFlagBits::eNone;

        vk::RenderingAttachmentInfo depth_attachment_info;
        depth_attachment_info.clearValue.depthStencil.depth = 1.0f;
        depth_attachment_info.clearValue.depthStencil.stencil = 0.0f;
        depth_attachment_info.imageLayout = vk::ImageLayout::eDepthStencilAttachmentOptimal;
        depth_attachment_info.imageView = _depth_attachment->depthstencil_imageview();
        depth_attachment_info.loadOp = vk::AttachmentLoadOp::eClear;
        depth_attachment_info.storeOp = vk::AttachmentStoreOp::eDontCare;
        depth_attachment_info.resolveMode = vk::ResolveModeFlagBits::eNone;

        const auto surf_size = vkctx().get_surface_size();

        vk::RenderingInfo rendering_info;
        rendering_info.colorAttachmentCount = 1;
        rendering_info.pColorAttachments = &color_attachment_info;
        rendering_info.layerCount = 1;
        rendering_info.pDepthAttachment = &depth_attachment_info;
        rendering_info.pStencilAttachment = &depth_attachment_info;
        rendering_info.renderArea.offset = vk::Offset2D(0, 0);
        rendering_info.renderArea.extent = vk::Extent2D(surf_size.x, surf_size.y);
        rendering_info.viewMask = 0;

        _render_cmdbuff->beginRendering(rendering_info);

        vk::Viewport viewport;
        viewport.x = 0;
        viewport.y = 0;
        viewport.width = surf_size.x;
        viewport.height = surf_size.y;
        viewport.minDepth = 0.0f;
        viewport.maxDepth = 1.0f;

        _render_cmdbuff->setViewport(0, viewport);

        vk::Rect2D scissor;
        scissor.offset = vk::Offset2D(0, 0);
        scissor.extent = vk::Extent2D(surf_size.x, surf_size.y);

        _render_cmdbuff->setScissor(0, scissor);
    }

    void renderer::submit_prerender_cmdbuffs()
    {
        _upload_queue->ready_for_submit();

        const auto image_ready_semaphore = _args.swapchain->image_ready_semaphore();
        const vk::PipelineStageFlags wait_stage_flags = vk::PipelineStageFlagBits::eAllCommands;

        const std::vector<vk::CommandBuffer> cmdbuffs = { _upload_queue->get_cmdbuff() };

        vk::SubmitInfo submit_info;
        submit_info.commandBufferCount = cmdbuffs.size();
        submit_info.pCommandBuffers = cmdbuffs.data();
        submit_info.signalSemaphoreCount = 1;
        submit_info.waitSemaphoreCount = 1;
        submit_info.pSignalSemaphores = &*_render_ready_semaphore;
        submit_info.pWaitSemaphores = &image_ready_semaphore;
        submit_info.pWaitDstStageMask = &wait_stage_flags;

        vkctx().graphics_queue().submit(submit_info);
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
            die("Failure presenting swapchain image");
        }
    }
} // namespace cathedral::engine