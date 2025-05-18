#include <cathedral/engine/camera.hpp>

#include <glm/ext/matrix_projection.hpp>
#include <glm/gtc/matrix_transform.hpp>

namespace cathedral::engine
{
    constexpr glm::vec3 FRONT_VEC = { 0, 0, 1 };
    constexpr glm::vec3 RIGHT_VEC = { 1, 0, 0 };

    void camera::set_world_position(const glm::vec3 pos)
    {
        if (_world_position != pos)
        {
            _view_needs_regen = true;
            _world_position = pos;
        }
    }

    void camera::set_world_rotation(const glm::vec3 rot)
    {
        if (_world_rotation != rot)
        {
            _view_needs_regen = true;
            _world_rotation = rot;
        }
    }

    void camera::translate(const glm::vec3 translation)
    {
        if (translation != glm::vec3(0, 0, 0))
        {
            _view_needs_regen = true;
            _world_position += translation;
        }
    }

    void camera::rotate_degrees(const glm::vec3 degrees)
    {
        if (degrees != glm::vec3(0, 0, 0))
        {
            _world_rotation += glm::radians(degrees);
            _view_needs_regen = true;
        }
    }

    void camera::rotate_radians(const glm::vec3 radians)
    {
        if (radians != glm::vec3(0, 0, 0))
        {
            _world_rotation += radians;
            _view_needs_regen = true;
        }
    }

    void camera::set_near_z(const float z)
    {
        _znear = z;
        _projection_needs_regen = true;
    }

    void camera::set_far_z(const float z)
    {
        _zfar = z;
        _projection_needs_regen = true;
    }

    glm::vec3 camera::forward() const
    {
        const auto& view = get_view_matrix();
        return glm::normalize(glm::vec3{ view[0][2], view[1][2], view[2][2] });
    }

    glm::vec3 camera::right() const
    {
        const auto& view = get_view_matrix();
        return glm::normalize(glm::vec3{ view[0][0], view[1][0], view[2][0] });
    }

    const glm::mat4& camera::get_view_matrix() const
    {
        if (_view_needs_regen)
        {
            auto rot = rotate(glm::mat4(1.0F), glm::radians(_world_rotation.x), glm::vec3{ -1, 0, 0 });
            rot = rotate(rot, glm::radians(_world_rotation.y), glm::vec3{ 0, 1, 0 });
            rot = rotate(rot, glm::radians(_world_rotation.z), glm::vec3{ 0, 0, 1 });
            _view = glm::translate(rot, _world_position * glm::vec3(-1, -1, -1));
            _view_needs_regen = false;
        }
        return _view;
    }

    const glm::mat4& orthographic_camera::get_projection_matrix() const
    {
        if (_projection_needs_regen)
        {
            _projection = glm::orthoLH(-1.0F, 1.0F, -1.0F, 1.0F, _znear, _zfar);
            _projection[1][1] *= -1; // invert y axis
            _projection_needs_regen = false;
        }
        return _projection;
    }

    const glm::mat4& perspective_camera::get_projection_matrix() const
    {
        if (_projection_needs_regen)
        {
            _projection = glm::perspective(glm::radians(_vfov), _aspect_ratio, _znear, _zfar);
            _projection[1][1] *= -1; // invert y axis
            _projection_needs_regen = false;
        }
        return _projection;
    }

    void perspective_camera::set_aspect_ratio(const float r)
    {
        if (_aspect_ratio != r)
        {
            _aspect_ratio = r;
            _projection_needs_regen = true;
        }
    }

    void perspective_camera::set_vertical_fov(const float fov)
    {
        _vfov = fov;
        _projection_needs_regen = true;
    }
} // namespace cathedral::engine
