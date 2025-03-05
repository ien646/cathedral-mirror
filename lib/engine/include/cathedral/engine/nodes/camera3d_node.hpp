#pragma once

#include <cathedral/engine/camera.hpp>
#include <cathedral/engine/nodes/node.hpp>

namespace cathedral::engine
{
    class camera3d_node : public node
    {
    public:
        using node::node;

        void set_main_camera(bool main);

        bool is_main_camera() const;

        perspective_camera& camera() { return _camera; }

        const perspective_camera& camera() const { return _camera; }

        void tick(scene& scn, double deltatime) override;

        constexpr const char* typestr() const override { return typestr_from_type(type()); }

        constexpr node_type type() const override { return node_type::CAMERA3D_NODE; }

    protected:
        bool _is_main_camera = false;
        perspective_camera _camera = { 60, 16.0F / 9.0F, 0.01F, 100.0F };
    };
} // namespace cathedral::engine