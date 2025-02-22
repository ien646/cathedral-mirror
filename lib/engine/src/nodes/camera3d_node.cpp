#include <cathedral/engine/nodes/camera3d_node.hpp>

#include <cathedral/engine/scene.hpp>

namespace cathedral::engine
{
    void camera3d_node::set_main_camera(bool main)
    {
        _is_main_camera = main;
    }

    void camera3d_node::tick(scene& scn, [[maybe_unused]] double deltatime)
    {
        const auto surf_size = scn.get_renderer().vkctx().get_surface_size();
        const float aspect_ratio = static_cast<float>(surf_size.x) / static_cast<float>(surf_size.y);
        _camera.set_position(_local_transform.position());
        _camera.set_rotation(_local_transform.rotation());
        _camera.set_aspect_ratio(aspect_ratio);
        if (_is_main_camera)
        {
            scn.update_uniform([&](scene_uniform_data& data) {
                data.projection3d = _camera.get_projection_matrix();
                data.view3d = _camera.get_view_matrix();
            });
        }
    }
} // namespace cathedral::engine