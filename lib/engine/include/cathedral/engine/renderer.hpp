#pragma once

#include <cathedral/gfx/depthstencil_attachment.hpp>
#include <cathedral/gfx/shader.hpp>
#include <cathedral/gfx/swapchain.hpp>
#include <cathedral/gfx/vulkan_context.hpp>

#include <cathedral/engine/material.hpp>
#include <cathedral/engine/texture.hpp>
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
        explicit renderer(renderer_args args);

        void begin_frame();
        void end_frame();

        inline uint64_t current_frame() const { return _frame_count; }

        void recreate_swapchain_dependent_resources();

        gfx::shader create_vertex_shader(std::string_view source) const;
        gfx::shader create_fragment_shader(std::string_view source) const;

        inline const gfx::vulkan_context& vkctx() const { return _args.swapchain->vkctx(); }

        inline const gfx::depthstencil_attachment& depthstencil_attachment() const { return *_depth_attachment; }

        inline vk::CommandBuffer render_cmdbuff() const { return *_render_cmdbuff; }

        inline const gfx::swapchain& swapchain() const { return *_args.swapchain; }

        upload_queue& get_upload_queue() { return *_upload_queue; }

        std::shared_ptr<texture> create_color_texture(
            const ien::image& img,
            uint32_t mip_levels = 8,
            vk::Filter min_filter = vk::Filter::eLinear,
            vk::Filter mag_filter = vk::Filter::eLinear,
            ien::resize_filter mipgen_filter = ien::resize_filter::BOX,
            vk::SamplerAddressMode address_mode = vk::SamplerAddressMode::eRepeat,
            uint32_t anisotropy = 8) const;

        std::shared_ptr<texture> create_color_texture(
            const std::string& image_path,
            uint32_t mip_levels = 8,
            vk::Filter min_filter = vk::Filter::eLinear,
            vk::Filter mag_filter = vk::Filter::eLinear,
            ien::resize_filter mipgen_filter = ien::resize_filter::BOX,
            vk::SamplerAddressMode address_mode = vk::SamplerAddressMode::eRepeat,
            uint32_t anisotropy = 8) const;

        std::shared_ptr<texture> default_texture() const { return _default_texture; }

        auto& materials() { return _materials; }

        const auto& materials() const { return _materials; }

        std::shared_ptr<material> create_material(const material_args& args);

        const auto& empty_uniform_buffer() const { return _empty_uniform_buffer; }

    private:
        renderer_args _args;

        uint32_t _swapchain_image_index = 0;
        uint64_t _frame_count = 0;

        std::unique_ptr<upload_queue> _upload_queue;

        std::unique_ptr<gfx::depthstencil_attachment> _depth_attachment;

        vk::UniqueFence _frame_fence;
        vk::UniqueSemaphore _render_ready_semaphore;
        vk::UniqueSemaphore _present_ready_semaphore;
        vk::UniqueCommandBuffer _render_cmdbuff;

        std::shared_ptr<texture> _default_texture;

        std::unordered_map<std::string, std::shared_ptr<material>> _materials;

        std::unique_ptr<gfx::uniform_buffer> _empty_uniform_buffer;

        void reload_depthstencil_attachment();

        void begin_rendercmd();

        void submit_prerender_cmdbuffs();
        void submit_render_cmdbuff();
        void submit_present();

        void init_default_texture();
        void init_empty_uniform_buffer();
    };
} // namespace cathedral::engine