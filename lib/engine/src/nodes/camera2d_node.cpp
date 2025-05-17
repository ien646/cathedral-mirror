#include <cathedral/engine/nodes/camera2d_node.hpp>

#include <cathedral/engine/scene.hpp>

namespace cathedral::engine
{
    void camera2d_node::tick(scene& scn, const double deltatime)
    {
        node::tick(scn, deltatime);

        if (_disabled || (_disabled_in_editor && scn.in_editor_mode()))
        {
            return;
        }

        const auto position = world_position();
        const auto rotation = world_rotation();

        if (position != _camera.position())
        {
            _camera.set_world_position(position);
        }
        if (rotation != _camera.rotation())
        {
            _camera.set_world_rotation(rotation);
        }
        if (_is_main_camera)
        {
            scn.update_uniform([&](scene_uniform_data& data) {
                data.projection2d = _camera.get_projection_matrix();
                data.view2d = _camera.get_view_matrix();
            });
        }
    }

    std::shared_ptr<scene_node> camera2d_node::copy(const std::string& copy_name, const bool copy_children) const
    {
        return copy_camera_node<camera2d_node>(copy_name, copy_children);
    }
} // namespace cathedral::engine