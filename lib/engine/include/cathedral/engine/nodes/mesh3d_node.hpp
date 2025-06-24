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

    constexpr auto MESH3D_NODE_TYPESTR = "mesh3d_node";

    class mesh3d_node final : public node
    {
    public:
        using node::node;

        void set_mesh(std::optional<std::string> name);
        void set_mesh(std::shared_ptr<mesh_buffer> mesh_buffer);

        void set_material(std::optional<std::string> name);

        std::optional<std::string> mesh_name() const { return _mesh_name; }

        std::optional<std::string> material_name() const { return _material_name; }

        auto get_material() const { return _material; }

        void bind_node_texture_slot(const std::string& texture_name, uint32_t slot);

        const std::vector<std::string>& texture_names() const { return _texture_names; }

        const std::vector<std::shared_ptr<texture>>& bound_textures() const { return _texture_slots; }

        void tick_setup(scene& scene) override;

        void tick(scene& scene, double deltatime) override;

        void editor_tick(scene& scene, double deltatime) override;

        std::shared_ptr<scene_node> copy(const std::string& name, bool copy_children) const override;

        void update_uniform(const std::function<void(std::span<std::byte>&)>& func);

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
        void set_node_variable_value(const std::string& name, const T& value)
        {
            if (_material.expired())
            {
                log_warning("Skipping node variable update since material is not present");
                return;
            }

            const auto& mat = _material.lock();
            const auto& offset_opt = mat->get_node_binding_var_offset(name);

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

                std::memcpy(data.data() + offset, reinterpret_cast<const void*>(&value), update_size);
            });
        }

        constexpr const char* typestr() const override { return typestr_from_type(type()); }

        constexpr node_type type() const override { return node_type::MESH3D_NODE; }

    protected:
        std::optional<std::string> _mesh_name;
        std::shared_ptr<mesh_buffer> _mesh_buffers;
        std::shared_ptr<engine::mesh> _mesh;
        bool _needs_update_mesh = true;
        std::optional<std::string> _material_name;
        bool _needs_update_material = true;
        std::unique_ptr<gfx::uniform_buffer> _mesh3d_uniform_buffer;
        std::weak_ptr<material> _material;
        uint32_t _material_uid = std::numeric_limits<uint32_t>::max();
        vk::UniqueDescriptorSet _descriptor_set;
        std::vector<std::string> _texture_names;
        std::vector<std::shared_ptr<texture>> _texture_slots;
        bool _needs_update_textures = true;

        std::vector<std::byte> _uniform_data;
        bool _uniform_needs_update = true;

        void init_default_textures(const renderer& rend);

        void update_material(scene& scene);

        void update_textures(scene& scene);

        void update_bindings();

        void bind_node_texture_slot(const renderer& rend, std::shared_ptr<texture>, uint32_t slot);

        void render(scene& scene);
    };
} // namespace cathedral::engine