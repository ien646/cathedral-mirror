#pragma once

#include <cathedral/engine/camera.hpp>
#include <cathedral/engine/node_type.hpp>
#include <cathedral/engine/nodes/bits/camera_node.hpp>

namespace cathedral::engine
{
    class camera2d_node final : public internal::camera_node_base<orthographic_camera, node_type::CAMERA2D_NODE>
    {
    public:
        using camera_node_base::camera_node_base;

        void tick(scene& scene, double deltatime) override;

        void editor_tick(scene& scene, double deltatime) override;

        std::shared_ptr<scene_node> copy(const std::string& copy_name, bool copy_children) const override;

    private:
        void update_data(scene& scene);
    };

    template<>
    std::shared_ptr<camera2d_node> construct_node<camera2d_node>(std::string name, scene_node* parent, bool enabled);
} // namespace cathedral::engine