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

    constexpr const char* MESH3D_NODE_TYPESTR = "mesh3d_node";

    class mesh3d_node : public node
    {
    public:
        using node::node;

        void set_mesh(std::optional<std::string> name);
        void set_mesh(std::shared_ptr<mesh_buffer> mesh_buffer);

        void set_material(std::optional<std::string> name);

        std::optional<std::string> mesh_name() const { return _mesh_name; }

        auto get_material() const { return _material; }

        void bind_node_texture_slot(const std::string& texture_name, uint32_t slot);

        const std::vector<std::shared_ptr<texture>>& bound_textures() const { return _texture_slots; }

        void tick_setup(scene& scene) override;

        void tick(scene& scene, double deltatime) override;

        std::shared_ptr<scene_node> copy(const std::string& name, bool copy_children) const override;

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
    };
} // namespace cathedral::engine