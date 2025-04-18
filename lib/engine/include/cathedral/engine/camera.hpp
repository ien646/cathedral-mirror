#pragma once

#include <cathedral/core.hpp>

#include <glm/mat4x4.hpp>
#include <glm/vec3.hpp>

namespace cathedral::engine
{
    class camera
    {
    public:
        constexpr camera() = default;

        constexpr camera(glm::vec3 initial_pos, glm::vec3 initial_rotation, float znear = 0.1F, float zfar = 100.0F) noexcept
            : _position(initial_pos)
            , _rotation(initial_rotation)
            , _znear(znear)
            , _zfar(zfar)
        {
        }

        virtual ~camera() noexcept = default;

        glm::vec3 position() const { return _position; }

        void set_position(glm::vec3);

        glm::vec3 rotation() const { return _rotation; }

        void set_rotation(glm::vec3);

        void translate(glm::vec3 translation);
        void rotate_radians(glm::vec3 radians);
        void rotate_degrees(glm::vec3 degrees);

        glm::vec3 get_front_vector() const;
        glm::vec3 get_right_vector() const;

        float near_z() const { return _znear; }

        float far_z() const { return _zfar; }

        void set_near_z(float z);
        void set_far_z(float z);

        float depth_magnitude() const { return _zfar - _znear; }

        const glm::mat4& get_view_matrix();
        virtual const glm::mat4& get_projection_matrix() = 0;

    protected:
        glm::vec3 _position = { 0, 0, 0 };
        glm::vec3 _rotation = { 0, 0, 0 };

        glm::mat4 _view = glm::mat4(1.0F);
        glm::mat4 _projection = glm::mat4(1.0F);
        bool _view_needs_regen = true;
        bool _projection_needs_regen = true;
        float _znear = 0.1F, _zfar = 100.0F;
    };

    class orthographic_camera final : public camera
    {
    public:
        constexpr orthographic_camera() = default;

        constexpr orthographic_camera(
            float znear,
            float zfar,
            glm::vec3 init_pos = { 0, 0, 0 },
            glm::vec3 init_rot = { 0, 0, 0 }) noexcept
            : camera(init_pos, init_rot, znear, zfar)
        {
        }

        const glm::mat4& get_projection_matrix() override;
    };

    class perspective_camera final : public camera
    {
    public:
        constexpr perspective_camera() = default;

        constexpr perspective_camera(
            float vertical_fov,
            float aspect_ratio,
            float znear,
            float zfar,
            glm::vec3 init_pos = { 0, 0, 0 },
            glm::vec3 init_rot = { 0, 0, 0 }) noexcept
            : camera(init_pos, init_rot, znear, zfar)
            , _vfov(vertical_fov)
            , _aspect_ratio(aspect_ratio)
        {
        }

        const glm::mat4& get_projection_matrix() override;

        float vertical_fov() const { return _vfov; }

        float aspect_ratio() const { return _aspect_ratio; }

        void set_aspect_ratio(float ratio);
        void set_vertical_fov(float fov);

    private:
        float _vfov = 60;
        float _aspect_ratio = 1.0F;
    };
} // namespace cathedral::engine
