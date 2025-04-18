#include <cathedral/engine/nodes/camera3d_node.hpp>

#include <cathedral/engine/scene.hpp>

namespace cathedral::engine
{
    void camera3d_node::tick(scene& scn, [[maybe_unused]] double deltatime)
    {
        const auto surf_size = scn.get_renderer().vkctx().get_surface_size();
        const float aspect_ratio = static_cast<float>(surf_size.x) / static_cast<float>(surf_size.y);
        if (_local_transform.position() != _camera.position())
        {
            _camera.set_position(_local_transform.position());
        }
        if (_local_transform.rotation() != _camera.rotation())
        {
            _camera.set_rotation(_local_transform.rotation());
        }
        if (aspect_ratio != _camera.aspect_ratio())
        {
            _camera.set_aspect_ratio(aspect_ratio);
        }
        if (_is_main_camera)
        {
            scn.update_uniform([&](scene_uniform_data& data) {
                data.projection3d = _camera.get_projection_matrix();
                data.view3d = _camera.get_view_matrix();
            });
        }
    }

    std::shared_ptr<scene_node> camera3d_node::copy(const std::string& copy_name, bool copy_children) const
    {
        return camera_node_base::copy<camera3d_node>(copy_name, copy_children);
    }
} // namespace cathedral::engine