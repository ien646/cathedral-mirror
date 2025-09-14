#pragma once

#include <cathedral/engine/engine_settings.hpp>
#include <cathedral/engine/material.hpp>
#include <cathedral/engine/render_domain.hpp>
#include <cathedral/engine/texture.hpp>
#include <cathedral/engine/upload_queue.hpp>
#include <cathedral/gfx/depthstencil_attachment.hpp>
#include <cathedral/gfx/shader.hpp>
#include <cathedral/gfx/swapchain.hpp>

namespace cathedral::engine
{
    struct renderer_args
    {
        gfx::swapchain* swapchain = nullptr;
        std::shared_ptr<engine_settings_interface> engine_settings;
    };

    class renderer
    {
    public:
        void init_semaphores();
        void init_commandbuffers();
        explicit renderer(renderer_args args);

        CATHEDRAL_NON_COPYABLE(renderer);
        CATHEDRAL_DEFAULT_MOVABLE(renderer);

        void begin_frame();
        void end_frame();

        uint64_t current_frame() const { return _frame_count; }

        void recreate_swapchain_dependent_resources();

        const gfx::vulkan_context& vkctx() const { return _args.swapchain->vkctx(); }

        const gfx::depthstencil_attachment& depthstencil_attachment() const { return *_depth_attachment; }

        vk::CommandBuffer render_cmdbuff(const render_domain type) const
        {
            using enum render_domain;
            switch (type)
            {
            case OPAQUE:
                return *_render_cmdbuff_opaque;
            case TRANSPARENT:
                return *_render_cmdbuff_transparent;
            case OVERLAY:
                return *_render_cmdbuff_overlay;
            }
            std::unreachable();
        }

        const gfx::swapchain& swapchain() const { return *_args.swapchain; }

        upload_queue& get_upload_queue() const { return *_upload_queue; }

        [[nodiscard]] std::shared_ptr<texture> create_color_texture(
            std::string name,
            const ien::image& img,
            uint32_t mip_levels = 8,
            vk::Filter min_filter = vk::Filter::eLinear,
            vk::Filter mag_filter = vk::Filter::eLinear,
            ien::resize_filter mipgen_filter = ien::resize_filter::BOX,
            vk::SamplerAddressMode address_mode = vk::SamplerAddressMode::eRepeat,
            uint32_t anisotropy = 8);

        [[nodiscard]] std::shared_ptr<texture> create_color_texture(
            std::string name,
            const std::string& image_path,
            uint32_t mip_levels = 8,
            vk::Filter min_filter = vk::Filter::eLinear,
            vk::Filter mag_filter = vk::Filter::eLinear,
            ien::resize_filter mipgen_filter = ien::resize_filter::BOX,
            vk::SamplerAddressMode address_mode = vk::SamplerAddressMode::eRepeat,
            uint32_t anisotropy = 8);

        [[nodiscard]] std::shared_ptr<texture> create_color_texture_from_data(const texture_args_from_data& args);

        [[nodiscard]] std::shared_ptr<texture> default_texture() const { return _default_texture; }

        [[nodiscard]] std::shared_ptr<gfx::storage_buffer> default_storage_buffer() const { return _default_storage_buffer; }

        auto& materials() { return _materials; }

        const auto& materials() const { return _materials; }

        const auto& textures() const { return _textures; }

        [[nodiscard]] std::weak_ptr<material> create_material(material_args args);

        const auto& empty_uniform_buffer() const { return _empty_uniform_buffer; }

        ien::image capture_screenshot() const;

        uint32_t uid() const { return _uid; }

        std::pair<glm::ivec2, glm::ivec2> custom_viewport() const;
        void set_custom_viewport(const std::optional<std::pair<glm::ivec2, glm::ivec2>>& rect);

        vk::SampleCountFlagBits msaa_samples() const { return _msaa_samples; }

        void enqueue_draw_command(render_domain domain, std::function<void(const vk::CommandBuffer& cmdbuff)>);

    private:
        renderer_args _args;
        uint32_t _uid;

        uint32_t _swapchain_image_index = 0;
        uint64_t _frame_count = 0;

        std::unique_ptr<upload_queue> _upload_queue;

        std::unique_ptr<gfx::depthstencil_attachment> _depth_attachment;

        vk::UniqueFence _frame_fence;
        vk::UniqueSemaphore _render_opaque_ready_semaphore;
        vk::UniqueSemaphore _render_transparent_ready_semaphore;
        vk::UniqueSemaphore _render_overlay_ready_semaphore;
        std::vector<vk::UniqueSemaphore> _present_ready_semaphore;

        vk::UniqueCommandBuffer _render_cmdbuff_opaque;
        vk::UniqueCommandBuffer _render_cmdbuff_transparent;
        vk::UniqueCommandBuffer _render_cmdbuff_overlay;

        std::unordered_map<render_domain, std::vector<std::function<void(const vk::CommandBuffer&)>>> _queued_draw_commands;

        std::shared_ptr<texture> _default_texture;
        std::shared_ptr<gfx::storage_buffer> _default_storage_buffer;

        std::unordered_map<std::string, std::shared_ptr<texture>> _textures;

        std::unordered_map<std::string, std::shared_ptr<material>> _materials;

        std::unique_ptr<gfx::uniform_buffer> _empty_uniform_buffer;

        std::optional<std::pair<glm::ivec2, glm::ivec2>> _custom_viewport = std::nullopt;

        std::unique_ptr<gfx::image> _main_render_target_image;
        vk::UniqueImageView _main_render_target_imageview;

        std::unique_ptr<settings::subscription> _msaa_setting_subscription;
        vk::SampleCountFlagBits _msaa_samples = vk::SampleCountFlagBits::e1;

        std::unique_ptr<settings::subscription> _msaa_sample_shading_subscription;
        bool _msaa_sample_shading = false;

        void reload_depthstencil_attachment() const;

        void begin_rendercmd();

        void submit_prerender_cmdbuffs();
        void submit_render_cmdbuff();
        void submit_present();

        void init_default_texture();
        void init_default_storage_buffer();
        void init_empty_uniform_buffer();
        void init_main_render_targets();
        void init_msaa();
        void init_upload_queue();

        void begin_opaque_pass(glm::ivec2 surf_size);
        void begin_transparent_pass(glm::ivec2 surf_size);
        void begin_overlay_pass(glm::ivec2 surf_size);
    };
} // namespace cathedral::engine