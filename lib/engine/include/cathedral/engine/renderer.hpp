#pragma once

#include <cathedral/gfx/depthstencil_attachment.hpp>
#include <cathedral/gfx/swapchain.hpp>
#include <cathedral/gfx/vulkan_context.hpp>

#include <cathedral/engine/upload_queue.hpp>

namespace cathedral::engine
{
    struct renderer_args
    {
        gfx::swapchain* swapchain = nullptr;
    };

    class renderer
    {
    public:
        renderer(renderer_args args);

        void begin_frame();
        void end_frame();

        void recreate_swapchain_dependent_resources();

        inline gfx::vulkan_context& vkctx() { return _args.swapchain->vkctx(); }

    private:
        renderer_args _args;

        uint32_t _swapchain_image_index;
        uint64_t _frame_count = 0;
        
        std::unique_ptr<upload_queue> _upload_queue;

        std::unique_ptr<gfx::depthstencil_attachment> _depth_attachment;

        vk::UniqueFence _frame_fence;
        vk::UniqueSemaphore _render_ready_semaphore;
        vk::UniqueSemaphore _present_ready_semaphore;
        vk::UniqueCommandBuffer _render_cmdbuff;

        void reload_depthstencil_attachment();

        void begin_rendercmd();

        void submit_prerender_cmdbuffs();
        void submit_render_cmdbuff();
        void submit_present();
    };
}