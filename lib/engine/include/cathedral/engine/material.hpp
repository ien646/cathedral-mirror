#pragma once

#include <cathedral/core.hpp>

#include <cathedral/engine/material_domain.hpp>
#include <cathedral/engine/shader.hpp>
#include <cathedral/engine/shader_bindings.hpp>
#include <cathedral/engine/shader_variable.hpp>

#include <cathedral/gfx/buffers/storage_buffer.hpp>
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

    using material_uniform_bindings_t = std::unordered_map<std::string, shader_material_uniform_binding>;
    using material_texture_bindings_t = std::unordered_map<std::string, shader_material_texture_binding>;
    using material_buffer_bindings_t = std::unordered_map<std::string, shader_material_buffer_binding>;
    using node_uniform_bindings_t = std::unordered_map<std::string, shader_node_uniform_binding>;
    using node_texture_bindings_t = std::unordered_map<std::string, shader_node_texture_binding>;
    using node_buffer_bindings_t = std::unordered_map<std::string, shader_node_buffer_binding>;

    struct material_args
    {
        std::string name;
        std::string vertex_shader_source;
        std::string fragment_shader_source;
        material_domain domain = material_domain::OPAQUE;
        material_uniform_bindings_t material_uniform_bindings;
        material_texture_bindings_t material_texture_bindings;
        material_buffer_bindings_t material_buffer_bindings;
        node_uniform_bindings_t node_uniform_bindings;
        node_texture_bindings_t node_texture_bindings;
        node_buffer_bindings_t node_buffer_bindings;
        bool wireframe = false;
        bool cull_backfaces = false;
        bool flip_front_faces = false;
    };

    class material
    {
    public:
        material(renderer* rend, material_args args);
        CATHEDRAL_NON_COPYABLE(material);
        CATHEDRAL_DEFAULT_MOVABLE(material);

        const std::string& name() const { return _args.name; }

        renderer& get_renderer() const { return *_renderer; }

        void bind_material_texture_slot(const std::string& name, uint32_t slot);

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

        const std::vector<std::shared_ptr<texture>>& bound_textures() const;

        const gfx::pipeline& pipeline() const;

        vk::DescriptorSetLayout material_descriptor_set_layout() const;
        vk::DescriptorSetLayout node_descriptor_set_layout() const;

        vk::DescriptorSet descriptor_set() const;

        const gfx::pipeline_descriptor_set& material_descriptor_set_definition() const;
        const gfx::pipeline_descriptor_set& node_descriptor_set_definition() const;

        std::shared_ptr<shader> vertex_shader() const;
        std::shared_ptr<shader> fragment_shader() const;

        material_domain domain() const;
        void set_domain(material_domain domain);

        uint32_t material_uniform_block_size() const;
        uint32_t material_texture_slots() const;
        const std::vector<std::string>& material_texture_names() const;

        uint32_t node_uniform_block_size() const;
        uint32_t node_texture_slots() const;
        const std::vector<std::string>& node_texture_names() const;

        const material_uniform_bindings_t& material_uniform_bindings() const;
        const material_texture_bindings_t& material_texture_bindings() const;
        const material_buffer_bindings_t& material_buffer_bindings() const;

        const node_uniform_bindings_t& node_uniform_bindings() const;
        const node_texture_bindings_t& node_texture_bindings() const;
        const node_buffer_bindings_t& node_buffer_bindings() const;

        const std::vector<shader_variable>& material_uniform_variables() const;
        const std::vector<shader_variable>& node_variables() const;

        void force_pipeline_update();
        void force_rebind_textures();

        template <concepts::ShaderVariableType T>
        void set_material_uniform_variable_value(const std::string& name, const T& value)
        {
            if (!_mat_var_offsets.contains(name))
            {
                return;
            }
            const auto offset = _mat_var_offsets[name];

            update_uniform([&](const std::span<std::byte>& data) {
                if (offset >= data.size_bytes())
                {
                    return;
                }

                const auto update_size = data.size_bytes() - offset;
                if (sizeof(T) > update_size)
                {
                    log_warning(std::format("Uniform update truncated! Material:{}, Var:{}", _args.name, name));
                }

                std::memcpy(data.data() + offset, reinterpret_cast<const void*>(&value), update_size);
            });
        }

        void set_material_uniform_binding_for_var(
            const std::string& var_name,
            std::optional<shader_material_uniform_binding> binding);
        void set_node_uniform_binding_for_var(const std::string& var_name, std::optional<shader_node_uniform_binding> binding);

        std::optional<uint32_t> get_material_uniform_var_offset(const std::string& var_name);
        std::optional<uint32_t> get_node_uniform_var_offset(const std::string& var_name);

        std::optional<uint32_t> get_material_buffer_index(const std::string& name) const;
        std::optional<uint32_t> get_node_buffer_index(const std::string& name) const;

        std::optional<uint32_t> get_material_texture_slot(const std::string& name) const;
        std::optional<uint32_t> get_node_texture_slot(const std::string& name) const;

        const std::vector<std::string>& material_buffer_names() const;
        const std::vector<std::string>& node_buffer_names() const;

        void set_storage_buffer_data(uint32_t binding_index, std::vector<std::byte> data);

        uint32_t uid() const { return _uid; }

        static material create_dummy_material(material_args args);

    protected:
        uint32_t _uid;
        renderer* _renderer;
        material_args _args;

        std::shared_ptr<shader> _vertex_shader;
        std::shared_ptr<shader> _fragment_shader;
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

        std::vector<std::shared_ptr<gfx::storage_buffer>> _storage_buffers;
        std::vector<std::vector<std::byte>> _storage_buffers_data;
        std::vector<bool> _storage_buffers_needs_update;

        void init_pipeline();
        void init_descriptor_set_layouts();
        void init_descriptor_set();
        void init_default_textures();

        void init_shaders_and_data();

        void update_storage_buffer(uint32_t binding_index);

    private:
        explicit material(material_args args)
            : _uid(std::numeric_limits<uint32_t>::max())
            , _renderer(nullptr)
            , _args(std::move(args))
        {
        }
    };
} // namespace cathedral::engine