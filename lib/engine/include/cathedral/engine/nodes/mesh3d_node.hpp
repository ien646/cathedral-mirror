#pragma once

#include <cathedral/engine/aligned_uniform.hpp>
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
        void set_mesh(std::shared_ptr<gfx::vertex_buffer> vertex_buffer);

        void set_material(world_geometry_material* mat);

        inline std::optional<std::string> mesh_name() const { return _mesh_path; }

        virtual void tick(double deltatime) override;

    protected:
        std::optional<std::string> _mesh_path;
        std::shared_ptr<gfx::vertex_buffer> _vertex_buffer;
        std::unique_ptr<gfx::uniform_buffer> _mesh3d_uniform_buffer;
        world_geometry_material* _material = nullptr;
        vk::UniqueDescriptorSet _descriptor_set;

        mesh3d_node_uniform_data _uniform_data;
        bool _uniform_needs_update = true;
    };
}