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

        perspective_camera& camera() { return _camera; }

        const perspective_camera& camera() const { return _camera; }

        void tick(scene& scn, double deltatime) override;

    protected:
        bool _is_main_camera = false;
        perspective_camera _camera = { 60, 16.0F / 9.0F, 0.01F, 100.0F };
    };
} // namespace cathedral::engine