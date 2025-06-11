#include <cathedral/engine/nodes/camera2d_node.hpp>

#include <cathedral/engine/scene.hpp>

namespace cathedral::engine
{
    void camera2d_node::tick(scene& scene, const double deltatime)
    {
        node::tick(scene, deltatime);
        if (_disabled)
        {
            return;
        }
        update_data(scene);
    }

    void camera2d_node::editor_tick(scene& scene, double deltatime)
    {
        node::editor_tick(scene, deltatime);
        if (_disabled || _disabled_in_editor)
        {
            return;
        }
        update_data(scene);
    }

    std::shared_ptr<scene_node> camera2d_node::copy(const std::string& copy_name, const bool copy_children) const
    {
        return copy_camera_node<camera2d_node>(copy_name, copy_children);
    }

    void camera2d_node::update_data(scene& scene)
    {
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
            scene.update_uniform([&](scene_uniform_data& data) {
                data.projection2d = _camera.get_projection_matrix();
                data.view2d = _camera.get_view_matrix();
            });
        }
    }
} // namespace cathedral::engine