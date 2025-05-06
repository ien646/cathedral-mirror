#include <cathedral/engine/camera.hpp>

#include <glm/ext/matrix_projection.hpp>
#include <glm/gtc/matrix_transform.hpp>

namespace cathedral::engine
{
    constexpr glm::vec3 FRONT_VEC = { 0, 0, 1 };
    constexpr glm::vec3 RIGHT_VEC = { 1, 0, 0 };

    void camera::set_position(const glm::vec3 pos)
    {
        _position = pos;
        _view_needs_regen = true;
    }

    void camera::set_rotation(const glm::vec3 rot)
    {
        _rotation = rot;
        _view_needs_regen = true;
    }

    void camera::translate(const glm::vec3 translation)
    {
        _position += translation;
        _view_needs_regen = true;
    }

    void camera::rotate_degrees(const glm::vec3 degrees)
    {
        _rotation += glm::radians(degrees);
        _view_needs_regen = true;
    }

    void camera::rotate_radians(const glm::vec3 radians)
    {
        _rotation += radians;
        _view_needs_regen = true;
    }

    glm::vec3 camera::get_front_vector() const
    {
        auto rotation_matrix = glm::rotate(glm::mat4{ 1.0F }, glm::radians(_rotation.x), glm::vec3{ 1, 0, 0 });
        rotation_matrix = glm::rotate(rotation_matrix, glm::radians(_rotation.y), glm::vec3{ 0, 1, 0 });
        rotation_matrix = glm::rotate(rotation_matrix, glm::radians(_rotation.z), glm::vec3{ 0, 0, 1 });
        return rotation_matrix * glm::vec4(FRONT_VEC, 1.0F);
    }

    glm::vec3 camera::get_right_vector() const
    {
        auto rotation_matrix = glm::rotate(glm::mat4{ 1.0F }, glm::radians(_rotation.x), glm::vec3{ 1, 0, 0 });
        rotation_matrix = glm::rotate(rotation_matrix, glm::radians(_rotation.y), glm::vec3{ 0, 1, 0 });
        rotation_matrix = glm::rotate(rotation_matrix, glm::radians(_rotation.z), glm::vec3{ 0, 0, 1 });
        return rotation_matrix * glm::vec4(RIGHT_VEC, 1.0F);
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

    const glm::mat4& camera::get_view_matrix()
    {
        if (_view_needs_regen)
        {
            auto rot = rotate(glm::mat4(1.0F), glm::radians(_rotation.x), glm::vec3{ -1, 0, 0 });
            rot = rotate(rot, glm::radians(_rotation.y), glm::vec3{ 0, 1, 0 });
            rot = rotate(rot, glm::radians(_rotation.z), glm::vec3{ 0, 0, 1 });
            _view = glm::translate(rot, _position * glm::vec3(-1, -1, -1));
            _view_needs_regen = false;
        }
        return _view;
    }

    const glm::mat4& orthographic_camera::get_projection_matrix()
    {
        if (_projection_needs_regen)
        {
            _projection = glm::orthoLH(-1.0F, 1.0F, -1.0F, 1.0F, _znear, _zfar);
            _projection[1][1] *= -1; //invert y axis
            _projection_needs_regen = false;
        }
        return _projection;
    }

    const glm::mat4& perspective_camera::get_projection_matrix()
    {
        if (_projection_needs_regen)
        {
            _projection = glm::perspective(glm::radians(_vfov), _aspect_ratio, _znear, _zfar);
            _projection[1][1] *= -1; //invert y axis
            _projection_needs_regen = false;
        }
        return _projection;
    }

    void perspective_camera::set_aspect_ratio(const float r)
    {
        _aspect_ratio = r;
        _projection_needs_regen = true;
    }

    void perspective_camera::set_vertical_fov(const float fov)
    {
        _vfov = fov;
        _projection_needs_regen = true;
    }
} // namespace cathedral::engine
