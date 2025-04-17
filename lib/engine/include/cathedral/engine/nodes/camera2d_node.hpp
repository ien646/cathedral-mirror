#pragma once

#include <cathedral/engine/camera.hpp>
#include <cathedral/engine/node_type.hpp>
#include <cathedral/engine/nodes/bits/camera_node.hpp>

namespace cathedral::engine
{
    class camera2d_node : public internal::camera_node_base<orthographic_camera, node_type::CAMERA2D_NODE>
    {
    public:
        using camera_node_base::camera_node_base;

        void tick(scene& scn, double deltatime) override;

        std::shared_ptr<scene_node> copy(const std::string& copy_name, bool copy_children) const override;
    };
} // namespace cathedral::engine