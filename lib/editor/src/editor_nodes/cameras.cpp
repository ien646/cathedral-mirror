#include <cathedral/editor/editor_nodes/cameras.hpp>

#include <cathedral/engine/scene.hpp>

#include <cathedral/engine/nodes/camera2d_node.hpp>
#include <cathedral/engine/nodes/camera3d_node.hpp>

namespace cathedral::editor::cameras
{
    constexpr auto NAME_2D = "__cathedral_editor_camera2d__";
    constexpr auto NAME_3D = "__cathedral_editor_camera3d__";

    namespace
    {
        void init_camera_to_defaults(auto& node)
        {
            node->set_local_position({ 0, 0, 0 });
            node->set_local_rotation({ 0, 0, 0 });
            node->set_local_scale({ 1, 1, 1 });
            node->set_enabled(true);
        }
    } // namespace

    std::shared_ptr<engine::camera2d_node> get_editor_camera2d_node(engine::scene& scene)
    {
        if (scene.contains_node(NAME_2D))
        {
            return std::dynamic_pointer_cast<engine::camera2d_node>(scene.get_node(NAME_2D));
        }
        auto node = scene.add_root_node<engine::camera2d_node>(NAME_2D);
        init_camera_to_defaults(node);
        return node;
    }

    std::shared_ptr<engine::camera3d_node> get_editor_camera3d_node(engine::scene& scene)
    {
        if (scene.contains_node(NAME_3D))
        {
            return std::dynamic_pointer_cast<engine::camera3d_node>(scene.get_node(NAME_3D));
        }

        auto node = scene.add_root_node<engine::camera3d_node>(NAME_3D);
        init_camera_to_defaults(node);
        return node;
    }
} // namespace cathedral::editor::cameras