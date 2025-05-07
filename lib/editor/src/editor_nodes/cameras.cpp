#include <cathedral/editor/editor_nodes/cameras.hpp>

#include <cathedral/engine/scene.hpp>

#include <cathedral/engine/nodes/camera2d_node.hpp>
#include <cathedral/engine/nodes/camera3d_node.hpp>

namespace cathedral::editor::cameras
{
    constexpr auto NAME_2D = "__cathedral_editor_camera2d__";
    constexpr auto NAME_3D = "__cathedral_editor_camera3d__";

    std::shared_ptr<engine::camera2d_node> get_editor_camera2d_node(engine::scene& scene)
    {
        if (scene.contains_node(NAME_2D))
        {
            return std::dynamic_pointer_cast<engine::camera2d_node>(scene.get_node(NAME_2D));
        }
        NOT_IMPLEMENTED();
    }

    std::shared_ptr<engine::camera3d_node> get_editor_camera3d_node(engine::scene& scene)
    {
        if (scene.contains_node(NAME_3D))
        {
            return std::dynamic_pointer_cast<engine::camera3d_node>(scene.get_node(NAME_3D));
        }
        NOT_IMPLEMENTED();
    }
} // namespace cathedral::editor::cameras