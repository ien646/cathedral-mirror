#pragma once

#include <cathedral/gfx/aligned_uniform.hpp>

#include <cathedral/engine/mesh_buffer_storage.hpp>
#include <cathedral/engine/nodes/node.hpp>
#include <cathedral/engine/texture.hpp>

#include <cathedral/gfx/buffers.hpp>

namespace cathedral::engine
{
    class material;

    class mesh3d_node : public node
    {
    public:
        mesh3d_node(scene& scn, const std::string& name, scene_node* parent = nullptr);

        void set_mesh(const std::string& path);
        void set_mesh(std::shared_ptr<mesh_buffer> mesh_buffer);

        void set_material(material* mat);

        std::optional<std::string> mesh_name() const { return _mesh_path; }

        material* get_material() { return _material; }

        const material* get_material() const { return _material; }

        void bind_node_texture_slot(std::shared_ptr<texture>, uint32_t slot);

        const std::vector<std::shared_ptr<texture>>& bound_textures() const { return _texture_slots; }

        void tick(double deltatime) override;

        constexpr const char* get_node_typestr() const override { return "mesh3d_node"; }

    protected:
        std::optional<std::string> _mesh_path;
        std::shared_ptr<mesh_buffer> _mesh_buffers;
        std::unique_ptr<gfx::uniform_buffer> _mesh3d_uniform_buffer;
        material* _material = nullptr;
        vk::UniqueDescriptorSet _descriptor_set;
        std::vector<std::shared_ptr<texture>> _texture_slots;

        std::vector<std::byte> _uniform_data;
        bool _uniform_needs_update = true;

        void init_default_textures();
    };
} // namespace cathedral::engine