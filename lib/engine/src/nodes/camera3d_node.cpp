#include <cathedral/engine/nodes/camera3d_node.hpp>

#include <cathedral/engine/scene.hpp>

#include <icecream.hpp>

namespace cathedral::engine
{
    void camera3d_node::tick(scene& scene, const double deltatime)
    {
        node::tick(scene, deltatime);
        if (_disabled)
        {
            return;
        }
        update_data(scene);
    }

    void camera3d_node::editor_tick(scene& scene, double deltatime)
    {
        node::editor_tick(scene, deltatime);
        if (_disabled || _disabled_in_editor)
        {
            return;
        }
        update_data(scene);
    }

    std::unique_ptr<scene_node> camera3d_node::copy(const std::string& copy_name, const bool copy_children) const
    {
        return copy_camera_node<camera3d_node>(copy_name, copy_children);
    }

    void camera3d_node::update_data(scene& scene)
    {
        const auto surf_size = scene.get_renderer().vkctx().get_surface_size();
        const float aspect_ratio = static_cast<float>(surf_size.x) / static_cast<float>(surf_size.y);

        const auto position = world_position();
        const auto rotation = world_rotation();

        _camera.set_world_position(position);
        _camera.set_world_rotation(rotation);
        _camera.set_aspect_ratio(aspect_ratio);

        if (_is_main_camera)
        {
            scene.update_uniform([&](scene_uniform_data& data) {
                data.projection3d = _camera.get_projection_matrix();
                data.view3d = _camera.get_view_matrix();
            });

            if (_parent != nullptr)
            {
                scene.set_main_camera_3d_node(dynamic_cast<camera3d_node*>(_parent->get_child(_name)));
            }
            else
            {
                scene.set_main_camera_3d_node(dynamic_cast<camera3d_node*>(scene.get_node(_name)));
            }
        }
    }

    template <>
    std::unique_ptr<camera3d_node> construct_node<camera3d_node>(std::string name, scene_node* parent, bool enabled)
    {
        return std::make_unique<camera3d_node>(std::move(name), parent, enabled);
    }
} // namespace cathedral::engine