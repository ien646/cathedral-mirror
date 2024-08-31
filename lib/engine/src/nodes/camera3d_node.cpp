#include <cathedral/engine/nodes/camera3d_node.hpp>

#include <cathedral/engine/scene.hpp>

namespace cathedral::engine
{
    camera3d_node::camera3d_node(scene& scn, const std::string& name, scene_node* parent)
        : node(scn, name, parent)
        , _camera(60, 16.0f / 9.0f, 0.0001f, 100.0f)
    {
    }

    void camera3d_node::set_main_camera(bool main)
    {
        _is_main_camera = true;
    }

    void camera3d_node::tick(double deltatime)
    {
        const auto surf_size = _scene.get_renderer().vkctx().get_surface_size();
        const float aspect_ratio = static_cast<float>(surf_size.x) / surf_size.y;
        _camera.set_position(_local_transform.position());
        _camera.set_rotation(_local_transform.rotation());
        _camera.set_aspect_ratio(aspect_ratio);
        if (_is_main_camera)
        {
            _scene.update_uniform([&](scene_uniform_data& data) {
                data.projection3d = _camera.get_projection_matrix();
                data.view3d = _camera.get_view_matrix();
            });
        }
    }
} // namespace cathedral::engine