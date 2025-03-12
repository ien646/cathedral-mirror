#pragma once

#include <cathedral/core.hpp>

#include <cathedral/engine/material_definition.hpp>

#include <memory>

namespace cathedral::engine
{
    gfx::vertex_input_description standard_vertex_input_description();

    class renderer;
    class texture;

    struct material_args
    {
        material_definition def;
        std::string name;
        std::shared_ptr<gfx::shader> vertex_shader;
        std::shared_ptr<gfx::shader> fragment_shader;
    };

    constexpr const char* STANDARD_VERTEX_INPUT_GLSLSTR = R"glsl(
layout (location = 0) in vec3 in_vertex_position;
layout (location = 1) in vec2 in_vertex_uv;
layout (location = 2) in vec3 in_vertex_normal;
layout (location = 3) in vec4 in_vertex_color;
)glsl";

    class material
    {
    public:
        material(renderer& rend, material_args args);

        const std::string& name() const { return _args.name; }

        const material_definition& definition() const { return _args.def; }

        renderer& get_renderer() { return _renderer; }

        void bind_material_texture_slot(const std::shared_ptr<texture>& tex, uint32_t slot);

        void update_uniform(const std::function<void(std::span<std::byte>)>& func);

        template <typename T>
        void update_uniform(const std::function<void(T&)>& func)
        {
            CRITICAL_CHECK(sizeof(T) <= _uniform_data.size());
            const auto previous_data = _uniform_data;
            func(*reinterpret_cast<T*>(_uniform_data.data()));
            if (previous_data != _uniform_data)
            {
                _uniform_needs_update = true;
            }
        }

        void update();

        const auto& bound_textures() const { return _texture_slots; }

        const gfx::pipeline& pipeline() const { return *_pipeline; }

        vk::DescriptorSetLayout material_descriptor_set_layout() const { return *_material_descriptor_set_layout; }

        vk::DescriptorSetLayout node_descriptor_set_layout() const { return *_node_descriptor_set_layout; }

        vk::DescriptorSet descriptor_set() const { return *_descriptor_set; }

        const auto& material_descriptor_set_definition() const { return _material_descriptor_set_info; }

        const auto& node_descriptor_set_definition() const { return _node_descriptor_set_info; }

        void set_vertex_shader(std::shared_ptr<gfx::shader> shader);

        void set_fragment_shader(std::shared_ptr<gfx::shader> shader);

        std::shared_ptr<gfx::shader> vertex_shader() const { return _args.vertex_shader; }

        std::shared_ptr<gfx::shader> fragment_shader() const { return _args.fragment_shader; }

        void force_pipeline_update();

        void force_rebind_textures();

    protected:
        renderer& _renderer;
        material_args _args;

        std::unique_ptr<gfx::pipeline> _pipeline;
        gfx::pipeline_descriptor_set _material_descriptor_set_info;
        gfx::pipeline_descriptor_set _node_descriptor_set_info;
        vk::UniqueDescriptorSetLayout _material_descriptor_set_layout;
        vk::UniqueDescriptorSetLayout _node_descriptor_set_layout;
        vk::UniqueDescriptorSet _descriptor_set;

        std::unique_ptr<gfx::uniform_buffer> _material_uniform;
        std::vector<std::shared_ptr<texture>> _texture_slots;

        std::vector<std::byte> _uniform_data;
        bool _uniform_needs_update = true;
        bool _needs_pipeline_update = false;

        void init_pipeline();
        void init_descriptor_set_layouts();
        void init_descriptor_set();
        void init_default_textures();
    };
} // namespace cathedral::engine