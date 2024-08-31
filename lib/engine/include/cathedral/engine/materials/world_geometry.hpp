#pragma once

#include <cathedral/gfx/pipeline.hpp>

#include <cathedral/engine/aligned_uniform.hpp>
#include <cathedral/engine/material.hpp>
#include <cathedral/engine/texture.hpp>

#include <glm/vec4.hpp>

namespace cathedral::engine
{
    struct world_geometry_material_args
    {
        const gfx::vulkan_context* vkctx = nullptr;
        const gfx::shader* vertex_shader = nullptr;
        const gfx::shader* fragment_shader = nullptr;
        vk::Format color_attachment_format = vk::Format::eUndefined;
        vk::Format depth_attachment_format = vk::Format::eUndefined;
        uint32_t material_texture_slots = 0;
    };

    struct world_geometry_material_uniform_data
    {
        CATHEDRAL_ALIGNED_UNIFORM(glm::vec4, tint) = { 1.0f, 1.0f, 1.0f, 1.0f };

        bool operator==(const world_geometry_material_uniform_data& rhs) const = default;
    };

    class world_geometry_material : public material
    {
    public:
        world_geometry_material(renderer& rend, world_geometry_material_args args);

        inline const gfx::pipeline& pipeline() const { return *_pipeline; }

        void update_uniform(std::function<void(world_geometry_material_uniform_data&)> func);

        void update() override;

        inline vk::DescriptorSet descriptor_set() const { return *_descriptor_set; }

        inline vk::DescriptorSetLayout material_descriptor_set_layout() const
        {
            return *_material_descriptor_set_layout;
        };

        inline vk::DescriptorSetLayout drawable_descriptor_set_layout() const
        {
            return *_drawable_descriptor_set_layout;
        };

        void bind_material_texture_slot(const texture& tex, uint32_t slot);

        static gfx::pipeline_descriptor_set material_descriptor_set_definition(world_geometry_material_args args);
        static gfx::pipeline_descriptor_set drawable_descriptor_set_definition();

    private:
        world_geometry_material_args _args;
        std::unique_ptr<gfx::pipeline> _pipeline;
        std::unique_ptr<gfx::uniform_buffer> _material_uniform;
        vk::UniqueDescriptorSetLayout _material_descriptor_set_layout;
        vk::UniqueDescriptorSetLayout _drawable_descriptor_set_layout;
        vk::UniqueDescriptorSet _descriptor_set;

        world_geometry_material_uniform_data _uniform_data;
        bool _uniform_needs_update = true;

        void init_pipeline();
        void init_descriptor_set_layouts();
        void init_descriptor_set();
        void init_default_textures();
    };
} // namespace cathedral::engine