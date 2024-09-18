#include <cathedral/engine/camera.hpp>

#include <glm/ext/matrix_projection.hpp>
#include <glm/gtc/matrix_transform.hpp>

namespace cathedral::engine
{
    constexpr glm::vec3 front_vec = { 0, 0, 1 };
    constexpr glm::vec3 right_vec = { 1, 0, 0 };

    void camera::set_position(glm::vec3 pos)
    {
        _position = pos;
        _view_needs_regen = true;
    }

    void camera::set_rotation(glm::vec3 rot)
    {
        _rotation = rot;
        _view_needs_regen = true;
    }

    void camera::translate(glm::vec3 translation)
    {
        _position += translation;
        _view_needs_regen = true;
    }

    void camera::rotate_degrees(glm::vec3 degrees)
    {
        _rotation += glm::radians(degrees);
        _view_needs_regen = true;
    }

    void camera::rotate_radians(glm::vec3 radians)
    {
        _rotation += radians;
        _view_needs_regen = true;
    }

    glm::vec3 camera::get_front_vector() const
    {
        auto rotation_matrix = glm::rotate(glm::mat4{ 1.0F }, glm::radians(_rotation.x), glm::vec3{ 1, 0, 0 });
        rotation_matrix = glm::rotate(rotation_matrix, glm::radians(_rotation.y), glm::vec3{ 0, 1, 0 });
        rotation_matrix = glm::rotate(rotation_matrix, glm::radians(_rotation.z), glm::vec3{ 0, 0, 1 });
        return rotation_matrix * glm::vec4(front_vec, 1.0F);
    }

    glm::vec3 camera::get_right_vector() const
    {
        auto rotation_matrix = glm::rotate(glm::mat4{ 1.0F }, glm::radians(_rotation.x), glm::vec3{ 1, 0, 0 });
        rotation_matrix = glm::rotate(rotation_matrix, glm::radians(_rotation.y), glm::vec3{ 0, 1, 0 });
        rotation_matrix = glm::rotate(rotation_matrix, glm::radians(_rotation.z), glm::vec3{ 0, 0, 1 });
        return rotation_matrix * glm::vec4(right_vec, 1.0F);
    }

    const glm::mat4& camera::get_view_matrix()
    {
        if (_view_needs_regen)
        {
            auto rot = rotate(glm::mat4(1.0F), glm::radians(_rotation.x), glm::vec3{ -1, 0, 0 });
            rot = rotate(rot, glm::radians(_rotation.y), glm::vec3{ 0, 1, 0 });
            rot = rotate(rot, glm::radians(_rotation.z), glm::vec3{ 0, 0, 1 });
            auto trans = glm::translate(glm::mat4(1.0F), _position * glm::vec3(-1, -1, -1));
            _view = rot * trans;
            _view_needs_regen = false;
        }
        return _view;
    }

    void orthographic_camera::set_bounds(float xmin, float xmax, float ymin, float ymax, float znear, float zfar)
    {
        if (_xmin != xmin || _xmax != xmax || _ymin != ymin || _znear != znear || _zfar != zfar)
        {
            _xmin = xmin;
            _xmax = xmax;
            _ymin = ymin;
            _ymax = ymax;
            _znear = znear;
            _zfar = zfar;
            _projection_needs_regen = true;
        }
    }

    const glm::mat4& orthographic_camera::get_projection_matrix()
    {
        if (_projection_needs_regen)
        {
            _projection = glm::ortho(0.0F, _xmax, _ymax, 0.0F, 0.0F, 10.0F);
            _projection_needs_regen = false;
        }
        return _projection;
    }

    const glm::mat4& perspective_camera::get_projection_matrix()
    {
        if (_projection_needs_regen)
        {
            _projection = glm::perspective(glm::radians(_vfov), _aspect_ratio, _znear, _zfar);
            _projection[1][1] *= -1;
            _projection_needs_regen = false;
        }
        return _projection;
    }

    void perspective_camera::set_aspect_ratio(float r)
    {
        _aspect_ratio = r;
        _projection_needs_regen = true;
    }

    void perspective_camera::set_vertical_fov(float fov)
    {
        _vfov = fov;
        _projection_needs_regen = true;
    }
} // namespace cathedral::engine
