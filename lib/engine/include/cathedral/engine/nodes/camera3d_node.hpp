#pragma once

#include <cathedral/engine/camera.hpp>
#include <cathedral/engine/nodes/node.hpp>

namespace cathedral::engine
{
    class camera3d_node : public node
    {
    public:
        camera3d_node(scene& scn, const std::string& name, scene_node* parent = nullptr);

        void set_main_camera(bool main);

        perspective_camera& camera() { return _camera; }
        const perspective_camera& camera() const { return _camera; }

        void tick(double deltatime) override;

    protected:
        bool _is_main_camera = false;
        perspective_camera _camera;
    };
}