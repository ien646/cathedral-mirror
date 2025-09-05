#pragma once

#include <cathedral/gfx/aligned_uniform.hpp>

#include <cathedral/engine/material.hpp>
#include <cathedral/engine/mesh_buffer_storage.hpp>
#include <cathedral/engine/nodes/node.hpp>
#include <cathedral/engine/texture.hpp>

#include <cathedral/gfx/buffers.hpp>

namespace cathedral::engine
{
    class material;

    class drawable_node : public node
    {
    public:
        using node::node;

        std::optional<std::string> mesh_name() const { return _mesh_name; }

        std::optional<std::string> material_name() const { return _material_name; }

        auto get_material() const { return _material; }

        void bind_node_texture_slot(const std::string& texture_name, uint32_t slot);

        const std::vector<std::string>& texture_names() const { return _texture_names; }

        const std::vector<std::shared_ptr<texture>>& bound_textures() const { return _texture_slots; }

        void tick_setup(scene& scene) override;

        void tick(scene& scene, double deltatime) override;

        void editor_tick(scene& scene, double deltatime) override;

        const std::vector<std::byte>& raw_uniform_data() const { return _uniform_data; }

        void set_raw_uniform_data(std::vector<std::byte> data) { _uniform_data = std::move(data); }

        void update_uniform(const std::function<void(std::span<std::byte>&)>& func);

        void force_refresh_uniform();

        void set_storage_buffer_data(uint32_t binding_index, std::vector<std::byte> data);

        template <typename T>
        void update_uniform(std::function<void(T&)> func)
        {
            CRITICAL_CHECK(sizeof(T) <= _uniform_data.size(), "Attempt to write beyond uniform data bounds");
            const auto previous_data = _uniform_data;
            func(*reinterpret_cast<T*>(_uniform_data.data()));
            if (previous_data != _uniform_data)
            {
                _uniform_needs_update = true;
            }
        }

        template <concepts::ShaderVariableType T>
        void set_node_uniform_variable_value(const std::string& name, const T& value)
        {
            if (_material.expired())
            {
                log_warning(std::format("Skipping node variable update since material is not present -> {}", name));
                return;
            }

            const auto& mat = _material.lock();
            const auto& offset_opt = mat->get_node_uniform_var_offset(name);

            if (!offset_opt.has_value())
            {
                return;
            }
            const auto offset = *offset_opt;

            update_uniform([&](const std::span<std::byte>& data) {
                if (offset >= data.size_bytes())
                {
                    return;
                }

                const auto update_size = data.size_bytes() - offset;
                if (sizeof(T) > update_size)
                {
                    log_warning(std::format("Uniform update truncated! Material:{}, Var:{}", mat->name(), name));
                }

                std::memcpy(data.data() + offset, reinterpret_cast<const void*>(&value), sizeof(T));
            });
        }

        template <concepts::ShaderVariableType T>
        T get_node_uniform_variable_value(const std::string& name) const
        {
            if (_material.expired())
            {
                log_warning("Defaulting node variable retrieval since material is not present");
                return T{};
            }

            const auto& mat = _material.lock();
            const auto& offset_opt = mat->get_node_uniform_var_offset(name);

            if (!offset_opt.has_value())
            {
                log_error("Invalid node variable retrieval. (Offset not found)");
                return T{};
            }
            const auto offset = *offset_opt;

            if (offset >= _uniform_data.size())
            {
                log_error("Invalid node variable retrieval. (Out of bounds)");
                return T{};
            }

            T result{};
            std::memcpy(&result, _uniform_data.data() + offset, sizeof(T));
            return result;
        }

        gfx::shader_data_value get_node_uniform_variable_value(const std::string& name, const gfx::shader_data_type type) const
        {
            switch (type)
            {
            case gfx::shader_data_type::BOOL:
                return get_node_uniform_variable_value<bool>(name);
            case gfx::shader_data_type::INT:
                return get_node_uniform_variable_value<int32_t>(name);
            case gfx::shader_data_type::UINT:
                return get_node_uniform_variable_value<uint32_t>(name);
            case gfx::shader_data_type::FLOAT:
                return get_node_uniform_variable_value<float>(name);
            case gfx::shader_data_type::DOUBLE:
                return get_node_uniform_variable_value<double>(name);
            case gfx::shader_data_type::BVEC2:
                return get_node_uniform_variable_value<glm::bvec2>(name);
            case gfx::shader_data_type::BVEC3:
                return get_node_uniform_variable_value<glm::bvec3>(name);
            case gfx::shader_data_type::BVEC4:
                return get_node_uniform_variable_value<glm::bvec4>(name);
            case gfx::shader_data_type::IVEC2:
                return get_node_uniform_variable_value<glm::ivec2>(name);
            case gfx::shader_data_type::IVEC3:
                return get_node_uniform_variable_value<glm::ivec3>(name);
            case gfx::shader_data_type::IVEC4:
                return get_node_uniform_variable_value<glm::ivec4>(name);
            case gfx::shader_data_type::VEC2:
                return get_node_uniform_variable_value<glm::vec2>(name);
            case gfx::shader_data_type::VEC3:
                return get_node_uniform_variable_value<glm::vec3>(name);
            case gfx::shader_data_type::VEC4:
                return get_node_uniform_variable_value<glm::vec4>(name);
            case gfx::shader_data_type::UVEC2:
                return get_node_uniform_variable_value<glm::uvec2>(name);
            case gfx::shader_data_type::UVEC3:
                return get_node_uniform_variable_value<glm::uvec3>(name);
            case gfx::shader_data_type::UVEC4:
                return get_node_uniform_variable_value<glm::uvec4>(name);
            case gfx::shader_data_type::DVEC2:
                return get_node_uniform_variable_value<glm::dvec2>(name);
            case gfx::shader_data_type::DVEC3:
                return get_node_uniform_variable_value<glm::dvec3>(name);
            case gfx::shader_data_type::DVEC4:
                return get_node_uniform_variable_value<glm::dvec4>(name);
            case gfx::shader_data_type::MAT2X2:
                return get_node_uniform_variable_value<glm::mat2x2>(name);
            case gfx::shader_data_type::MAT2X3:
                return get_node_uniform_variable_value<glm::mat2x3>(name);
            case gfx::shader_data_type::MAT2X4:
                return get_node_uniform_variable_value<glm::mat2x4>(name);
            case gfx::shader_data_type::MAT3X2:
                return get_node_uniform_variable_value<glm::mat3x2>(name);
            case gfx::shader_data_type::MAT3X3:
                return get_node_uniform_variable_value<glm::mat3x3>(name);
            case gfx::shader_data_type::MAT3X4:
                return get_node_uniform_variable_value<glm::mat3x4>(name);
            case gfx::shader_data_type::MAT4X2:
                return get_node_uniform_variable_value<glm::mat4x2>(name);
            case gfx::shader_data_type::MAT4X3:
                return get_node_uniform_variable_value<glm::mat4x3>(name);
            case gfx::shader_data_type::MAT4X4:
                return get_node_uniform_variable_value<glm::mat4x4>(name);
            default:
                CRITICAL_ERROR("Unhandled shader data type");
            }
        }

        void set_instance_count(uint32_t count);
        uint32_t instance_count() const;

    protected:
        std::optional<std::string> _mesh_name;
        std::shared_ptr<mesh_buffer> _mesh_buffers;
        std::shared_ptr<mesh> _mesh;
        bool _needs_update_mesh = true;

        std::optional<std::string> _material_name;
        std::weak_ptr<material> _material;
        uint32_t _material_uid = std::numeric_limits<uint32_t>::max();
        bool _needs_update_material = true;

        std::unique_ptr<gfx::uniform_buffer> _node_uniform_buffer;
        std::vector<std::byte> _uniform_data;
        bool _uniform_needs_update = true;

        std::vector<std::string> _texture_names;
        std::vector<std::shared_ptr<texture>> _texture_slots;
        bool _needs_update_textures = true;

        std::vector<std::shared_ptr<gfx::storage_buffer>> _node_storage_buffers;
        std::vector<std::vector<std::byte>> _node_storage_buffers_data;
        std::vector<bool> _needs_update_buffers;

        vk::UniqueDescriptorSet _descriptor_set;

        uint32_t _instance_count = 1;

        void init_default_textures(const renderer& rend);

        void update_storage_buffer(const scene& scene, uint32_t binding_index);
        void update_material(scene& scene);
        void update_textures(scene& scene);

        void update_bindings();

        void bind_node_texture_slot(const renderer& rend, std::shared_ptr<texture>, uint32_t slot);

        void set_mesh(std::optional<std::string> name);
        void set_mesh(std::shared_ptr<mesh_buffer> mesh_buffer);

        void set_material(std::optional<std::string> name);

        virtual void render(scene& scene) = 0;

        template <typename Target>
        std::unique_ptr<Target> copy_drawable(const std::string& name, const bool copy_children) const
        {
            auto result = std::make_unique<Target>(name, _parent, !_disabled);

            node::copy_into(*result, copy_children);

            if (_mesh_name.has_value())
            {
                result->set_mesh(_mesh_name);
            }
            result->set_material(_material_name);

            for (uint32_t i = 0; i < static_cast<uint32_t>(_texture_slots.size()); ++i)
            {
                result->bind_node_texture_slot(_texture_names[i], i);
            }

            result->_uniform_data = _uniform_data;

            return result;
        }
    };
} // namespace cathedral::engine