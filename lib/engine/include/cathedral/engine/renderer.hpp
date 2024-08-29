#pragma once

#include <cathedral/gfx/depthstencil_attachment.hpp>
#include <cathedral/gfx/shader.hpp>
#include <cathedral/gfx/swapchain.hpp>
#include <cathedral/gfx/vulkan_context.hpp>

#include <cathedral/engine/materials/world_geometry.hpp>
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

        inline uint64_t current_frame() const { return _frame_count; }

        void recreate_swapchain_dependent_resources();

        gfx::shader create_vertex_shader(const std::string& source) const;
        gfx::shader create_fragment_shader(const std::string& source) const;

        inline const gfx::vulkan_context& vkctx() const { return _args.swapchain->vkctx(); }
        inline const gfx::depthstencil_attachment& depthstencil_attachment() const { return *_depth_attachment; }

        inline vk::CommandBuffer render_cmdbuff() const { return *_render_cmdbuff; }

        upload_queue& get_upload_queue() { return *_upload_queue; }

        const world_geometry_material& create_world_geometry_material(
            const std::string& name,
            const gfx::shader& vertex_shader,
            const gfx::shader& fragment_shader,
            uint32_t material_texture_slots = 0);

        inline std::unordered_map<std::string, world_geometry_material>& world_materials() { return _world_materials; }
        inline const std::unordered_map<std::string, world_geometry_material>& world_materials() const { return _world_materials; }

    private:
        renderer_args _args;

        uint32_t _swapchain_image_index = 0;
        uint64_t _frame_count = 0;

        std::unique_ptr<upload_queue> _upload_queue;

        std::unique_ptr<gfx::depthstencil_attachment> _depth_attachment;

        std::unordered_map<std::string, world_geometry_material> _world_materials;

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
} // namespace cathedral::engine