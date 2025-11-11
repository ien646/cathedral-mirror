#pragma once

#include <cathedral/engine/material.hpp>
#include <cathedral/engine/nodes/bits/drawable_node.hpp>
#include <cathedral/engine/nodes/node.hpp>

namespace cathedral::engine
{
    class material;

    constexpr auto MESH3D_NODE_TYPESTR = "mesh3d_node";

    class mesh3d_node final : public drawable_node
    {
    public:
        using drawable_node::drawable_node;
        using drawable_node::set_material;
        using drawable_node::set_mesh;

        std::unique_ptr<scene_node> copy(const std::string& name, bool copy_children) const override;

        constexpr node_type type() const override { return node_type::from_chars("CE::msh3"); }

    protected:
        void render(scene& scene) override;
    };

    template <>
    std::unique_ptr<mesh3d_node> construct_node<mesh3d_node>(std::string name, scene_node* parent, bool enabled);
} // namespace cathedral::engine