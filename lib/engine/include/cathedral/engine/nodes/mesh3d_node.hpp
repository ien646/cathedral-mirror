#pragma once

#include <cathedral/engine/aligned_uniform.hpp>
#include <cathedral/engine/mesh_buffer_storage.hpp>
#include <cathedral/engine/texture.hpp>
#include <cathedral/engine/materials/world_geometry.hpp>
#include <cathedral/engine/nodes/node.hpp>

#include <cathedral/gfx/buffers.hpp>

namespace cathedral::engine
{
    struct mesh3d_node_uniform_data
    {
        CATHEDRAL_ALIGNED_UNIFORM(glm::mat4, model_matrix) = glm::mat4(1.0f);

        bool operator==(const mesh3d_node_uniform_data& rhs) const = default;
    };

    class mesh3d_node : public node
    {
    public:
        mesh3d_node(scene& scn, const std::string& name, scene_node* parent = nullptr);

        void set_mesh(const std::string& path);
        void set_mesh(std::shared_ptr<mesh_buffer> mesh_buffer);

        void set_material(world_geometry_material* mat);

        std::optional<std::string> mesh_name() const { return _mesh_path; }

        world_geometry_material* material() { return _material; }
        const world_geometry_material* material() const { return _material; }

        void bind_node_texture_slot(std::shared_ptr<texture>, uint32_t slot);
        const std::vector<std::shared_ptr<texture>>& bound_textures() const { return _texture_slots; }

        void tick(double deltatime) override;

    protected:
        std::optional<std::string> _mesh_path;
        std::shared_ptr<mesh_buffer> _mesh_buffers;
        std::unique_ptr<gfx::uniform_buffer> _mesh3d_uniform_buffer;
        world_geometry_material* _material = nullptr;
        vk::UniqueDescriptorSet _descriptor_set;
        std::vector<std::shared_ptr<texture>> _texture_slots;

        mesh3d_node_uniform_data _uniform_data;
        bool _uniform_needs_update = true;

        void init_default_textures();
    };
}