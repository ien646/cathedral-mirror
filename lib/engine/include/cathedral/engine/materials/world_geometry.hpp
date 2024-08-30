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
        const gfx::shader* vertex_shader = nullptr;
        const gfx::shader* fragment_shader = nullptr;
        uint32_t material_texture_slots = 0;
        uint32_t node_texture_slots = 0;
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

        const gfx::pipeline& pipeline() const { return *_pipeline; }

        void update_uniform(std::function<void(world_geometry_material_uniform_data&)> func);

        void update() override;

        vk::DescriptorSet descriptor_set() const { return *_descriptor_set; }

        vk::DescriptorSetLayout material_descriptor_set_layout() const { return *_material_descriptor_set_layout; };

        vk::DescriptorSetLayout node_descriptor_set_layout() const { return *_node_descriptor_set_layout; };

        void bind_material_texture_slot(std::shared_ptr<texture>, uint32_t slot);

        gfx::pipeline_descriptor_set material_descriptor_set_definition();
        gfx::pipeline_descriptor_set node_descriptor_set_definition();

        size_t material_texture_slot_count() const { return _args.material_texture_slots; }
        size_t node_texture_slot_count() const { return _args.node_texture_slots; }
        
        const std::vector<std::shared_ptr<texture>>& bound_textures() const { return _texture_slots; }

    private:
        world_geometry_material_args _args;
        std::unique_ptr<gfx::pipeline> _pipeline;
        std::unique_ptr<gfx::uniform_buffer> _material_uniform;
        vk::UniqueDescriptorSetLayout _material_descriptor_set_layout;
        vk::UniqueDescriptorSetLayout _node_descriptor_set_layout;
        vk::UniqueDescriptorSet _descriptor_set;
        std::vector<std::shared_ptr<texture>> _texture_slots;

        world_geometry_material_uniform_data _uniform_data;
        bool _uniform_needs_update = true;

        void init_pipeline();
        void init_descriptor_set_layouts();
        void init_descriptor_set();
        void init_default_textures();
    };
} // namespace cathedral::engine