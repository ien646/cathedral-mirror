#include <cathedral/engine/nodes/camera2d_node.hpp>

#include <cathedral/engine/scene.hpp>

namespace cathedral::engine
{
    void camera2d_node::tick(scene& scn, [[maybe_unused]] double deltatime)
    {
        if (_local_transform.position() != _camera.position())
        {
            _camera.set_position(_local_transform.position());
        }
        if (_local_transform.rotation() != _camera.rotation())
        {
            _camera.set_rotation(_local_transform.rotation());
        }
        if (_is_main_camera)
        {
            scn.update_uniform([&](scene_uniform_data& data) {
                data.projection2d = _camera.get_projection_matrix();
                data.view2d = _camera.get_view_matrix();
            });
        }
    }

    std::shared_ptr<scene_node> camera2d_node::copy(const std::string& copy_name, bool copy_children) const
    {
        return camera_node_base::copy<camera2d_node>(copy_name, copy_children);
    }
} // namespace cathedral::engine