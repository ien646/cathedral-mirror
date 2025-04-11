#pragma once

#include <cathedral/core.hpp>

#include <cathedral/engine/material_domain.hpp>
#include <cathedral/engine/shader.hpp>
#include <cathedral/engine/shader_bindings.hpp>
#include <cathedral/engine/shader_variable.hpp>

#include <cathedral/gfx/buffers/uniform_buffer.hpp>
#include <cathedral/gfx/pipeline.hpp>

#include <functional>
#include <memory>
#include <unordered_map>
#include <vector>

namespace cathedral::engine
{
    gfx::vertex_input_description standard_vertex_input_description();

    class renderer;
    class texture;

    struct material_args
    {
        std::string name;
        std::string vertex_shader_source;
        std::string fragment_shader_source;
        material_domain domain = material_domain::OPAQUE;
        std::unordered_map<shader_material_uniform_binding, std::string> material_bindings;
        std::unordered_map<shader_node_uniform_binding, std::string> node_bindings;
    };

    class material
    {
    public:
        material(renderer* rend, material_args args);
        CATHEDRAL_NON_COPYABLE(material);
        CATHEDRAL_DEFAULT_MOVABLE(material);

        const std::string& name() const { return _args.name; }

        renderer& get_renderer() { return *_renderer; }

        void bind_material_texture_slot(const std::shared_ptr<texture>& tex, uint32_t slot);

        void update_uniform(const std::function<void(std::span<std::byte>&)>& func);

        template <typename T>
        void update_uniform(const std::function<void(T&)>& func)
        {
            CRITICAL_CHECK(sizeof(T) <= _uniform_data.size(), "Attempt to write beyond uniform data bounds");
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

        std::shared_ptr<engine::shader> vertex_shader() const { return _vertex_shader; }

        std::shared_ptr<engine::shader> fragment_shader() const { return _fragment_shader; }

        material_domain domain() const { return _args.domain; }

        void set_domain(material_domain domain) { _args.domain = domain; }

        uint32_t material_uniform_block_size() const { return _material_uniform_block_size; }

        uint32_t material_texture_slots() const { return static_cast<uint32_t>(_merged_pp_data.material_textures.size()); }

        const auto& material_texture_names() const { return _merged_pp_data.material_textures; }

        uint32_t node_uniform_block_size() const { return _node_uniform_block_size; }

        uint32_t node_texture_slots() const { return static_cast<uint32_t>(_merged_pp_data.node_textures.size()); }

        const auto& node_texture_names() const { return _merged_pp_data.node_textures; }

        const auto& material_bindings() const { return _args.material_bindings; }

        const auto& node_bindings() const { return _args.node_bindings; }

        const auto& material_variables() const { return _merged_pp_data.material_vars; }

        const auto& node_variables() const { return _merged_pp_data.node_vars; }

        void force_pipeline_update();

        void force_rebind_textures();

        template <concepts::ShaderVariableType T>
        void set_material_variable_value(const std::string& name, const T& value)
        {
            if (!_mat_var_offsets.contains(name))
            {
                return;
            }
            const auto offset = _mat_var_offsets[name];

            update_uniform([&](std::span<std::byte>& data) {
                if (offset >= data.size_bytes())
                {
                    return;
                }

                const auto update_size = data.size_bytes() - offset;
                if (sizeof(T) > update_size)
                {
                    debug_log(std::format("Uniform update truncated! Material:{}, Var:{}", _args.name, name));
                }

                std::memcpy(data.data() + offset, reinterpret_cast<const void*>(&value), update_size);
            });
        }

        void set_material_binding_for_var(const std::string& var_name, std::optional<shader_material_uniform_binding> binding);
        void set_node_binding_for_var(const std::string& var_name, std::optional<shader_node_uniform_binding> binding);

        uint32_t get_material_binding_var_offset(const std::string& var_name);
        uint32_t get_node_binding_var_offset(const std::string& var_name);

        uint32_t uid() const { return _uid; }

    protected:
        uint32_t _uid;
        renderer* _renderer;
        material_args _args;

        std::shared_ptr<engine::shader> _vertex_shader;
        std::shared_ptr<engine::shader> _fragment_shader;
        shader_preprocess_data _merged_pp_data;
        uint32_t _material_uniform_block_size = 0;
        uint32_t _node_uniform_block_size = 0;

        std::unique_ptr<gfx::pipeline> _pipeline;
        gfx::pipeline_descriptor_set _material_descriptor_set_info;
        gfx::pipeline_descriptor_set _node_descriptor_set_info;
        vk::UniqueDescriptorSetLayout _material_descriptor_set_layout;
        vk::UniqueDescriptorSetLayout _node_descriptor_set_layout;
        vk::UniqueDescriptorSet _descriptor_set;

        std::unordered_map<std::string, uint32_t> _mat_var_offsets;
        std::unordered_map<std::string, uint32_t> _node_var_offsets;

        std::unique_ptr<gfx::uniform_buffer> _material_uniform;
        std::vector<std::shared_ptr<texture>> _texture_slots;

        std::vector<std::byte> _uniform_data;
        bool _uniform_needs_update = true;
        bool _needs_pipeline_update = false;

        void init_pipeline();
        void init_descriptor_set_layouts();
        void init_descriptor_set();
        void init_default_textures();

        void init_shaders_and_data();
    };
} // namespace cathedral::engine