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
        using drawable_node::set_mesh;

        std::shared_ptr<scene_node> copy(const std::string& name, bool copy_children) const override;

        constexpr const char* typestr() const override { return typestr_from_type(type()); }

        constexpr node_type type() const override { return node_type::MESH3D_NODE; }

    protected:
        void render(scene& scene) override;
    };
} // namespace cathedral::engine