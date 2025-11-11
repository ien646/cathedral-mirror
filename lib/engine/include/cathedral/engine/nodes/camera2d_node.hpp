#pragma once

#include <cathedral/engine/camera.hpp>
#include <cathedral/engine/node_type.hpp>
#include <cathedral/engine/nodes/bits/camera_node.hpp>

namespace cathedral::engine
{
    class camera2d_node : public internal::camera_node_base<orthographic_camera>
    {
    public:
        using camera_node_base::camera_node_base;

        void tick(scene& scene, double deltatime) override;

        void editor_tick(scene& scene, double deltatime) override;

        std::unique_ptr<scene_node> copy(const std::string& copy_name, bool copy_children) const override;

        constexpr node_type type() const override { return node_type::from_chars("CE::cam2"); }

    private:
        void update_data(scene& scene);
    };

    template <>
    std::unique_ptr<camera2d_node> construct_node<camera2d_node>(std::string name, scene_node* parent, bool enabled);
} // namespace cathedral::engine