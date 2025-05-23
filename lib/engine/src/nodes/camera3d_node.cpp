#include <cathedral/engine/nodes/camera3d_node.hpp>

#include <cathedral/engine/scene.hpp>

#include <icecream.hpp>

namespace cathedral::engine
{
    void camera3d_node::tick(scene& scn, const double deltatime)
    {
        node::tick(scn, deltatime);

        if (_disabled || (_disabled_in_editor && scn.in_editor_mode()))
        {
            return;
        }

        const auto surf_size = scn.get_renderer().vkctx().get_surface_size();
        const float aspect_ratio = static_cast<float>(surf_size.x) / static_cast<float>(surf_size.y);

        const auto position = world_position();
        const auto rotation = world_rotation();

        _camera.set_world_position(position);
        _camera.set_world_rotation(rotation);
        _camera.set_aspect_ratio(aspect_ratio);

        if (_is_main_camera)
        {
            scn.update_uniform([&](scene_uniform_data& data) {
                data.projection3d = _camera.get_projection_matrix();
                data.view3d = _camera.get_view_matrix();
            });
        }
    }

    std::shared_ptr<scene_node> camera3d_node::copy(const std::string& copy_name, const bool copy_children) const
    {
        return copy_camera_node<camera3d_node>(copy_name, copy_children);
    }
} // namespace cathedral::engine