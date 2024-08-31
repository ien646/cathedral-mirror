#pragma once

#include <cathedral/core.hpp>

#include <glm/mat4x4.hpp>
#include <glm/vec3.hpp>

namespace cathedral::engine
{
    class camera
    {
    public:
        constexpr camera(glm::vec3 initial_pos = { 0, 0, 0 }, glm::vec3 initial_rotation = { 0, 0, 0 }) noexcept
            : _position(initial_pos)
            , _rotation(initial_rotation)
        {
        }

        virtual ~camera() noexcept = default;

        inline glm::vec3 position() const { return _position; }

        void set_position(glm::vec3);

        inline glm::vec3 rotation() const { return _rotation; }

        void set_rotation(glm::vec3);

        void translate(glm::vec3 translation);
        void rotate_radians(glm::vec3 radians);
        void rotate_degrees(glm::vec3 degrees);

        glm::vec3 get_front_vector() const;
        glm::vec3 get_right_vector() const;

        const glm::mat4& get_view_matrix();
        virtual const glm::mat4& get_projection_matrix() = 0;

    protected:
        glm::vec3 _position = { 0, 0, 0 };
        glm::vec3 _rotation = { 0, 0, 0 };

        glm::mat4 _view = glm::mat4(1.0F);
        glm::mat4 _projection = glm::mat4(1.0F);
        bool _view_needs_regen = true;
        bool _projection_needs_regen = true;
    };

    class orthographic_camera final : public camera
    {
    public:
        constexpr orthographic_camera(
            float xmin,
            float xmax,
            float ymin,
            float ymax,
            float znear,
            float zfar,
            glm::vec3 init_pos = { 0, 0, 0 },
            glm::vec3 init_rot = { 0, 0, 0 }) noexcept
            : camera(init_pos, init_rot)
            , _xmin(xmin)
            , _xmax(xmax)
            , _ymin(ymin)
            , _ymax(ymax)
            , _znear(znear)
            , _zfar(zfar)
        {
        }

        inline float width() const { return _xmax - _xmin; }

        inline float height() const { return _ymax - _ymin; }

        inline float depth_magnitude() const { return _zfar - _znear; }

        inline float znear() const { return _znear; }

        inline float zfar() const { return _zfar; }

        void set_bounds(float xmin, float xmax, float ymin, float ymax, float znear, float zfar);

        const glm::mat4& get_projection_matrix() override;

    private:
        float _xmin, _xmax, _ymin, _ymax, _znear, _zfar;
    };

    class perspective_camera final : public camera
    {
    public:
        constexpr perspective_camera(
            float vertical_fov,
            float aspect_ratio,
            float znear,
            float zfar,
            glm::vec3 init_pos = { 0, 0, 0 },
            glm::vec3 init_rot = { 0, 0, 0 }) noexcept
            : camera(init_pos, init_rot)
            , _vfov(vertical_fov)
            , _aspect_ratio(aspect_ratio)
            , _znear(znear)
            , _zfar(zfar)
        {
        }

        const glm::mat4& get_projection_matrix() override;

        inline float vertical_fov() const { return _vfov; }

        inline float aspect_ratio() const { return _aspect_ratio; }

        inline float near_z() const { return _znear; }

        inline float far_z() const { return _zfar; }

        void set_aspect_ratio(float ratio);
        void set_vertical_fov(float fov);

    private:
        float _vfov;
        float _aspect_ratio;
        float _znear, _zfar;
    };
} // namespace cathedral::engine
