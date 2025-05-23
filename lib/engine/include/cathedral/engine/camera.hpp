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

        constexpr camera(
            const glm::vec3 initial_pos,
            const glm::vec3 initial_rotation,
            const float znear = 0.1F,
            const float zfar = 100.0F) noexcept
            : _world_position(initial_pos)
            , _world_rotation(initial_rotation)
            , _znear(znear)
            , _zfar(zfar)
        {
        }

        virtual ~camera() noexcept = default;

        glm::vec3 position() const { return _world_position; }

        void set_world_position(glm::vec3);

        glm::vec3 rotation() const { return _world_rotation; }

        void set_world_rotation(glm::vec3);

        void translate(glm::vec3 translation);
        void rotate_radians(glm::vec3 radians);
        void rotate_degrees(glm::vec3 degrees);

        float near_z() const { return _znear; }

        float far_z() const { return _zfar; }

        void set_near_z(float z);
        void set_far_z(float z);

        float depth_magnitude() const { return _zfar - _znear; }

        glm::vec3 forward() const;
        glm::vec3 right() const;

        const glm::mat4& get_view_matrix() const;
        virtual const glm::mat4& get_projection_matrix() const = 0;

    protected:
        glm::vec3 _world_position = { 0, 0, 0 };
        glm::vec3 _world_rotation = { 0, 0, 0 };

        mutable glm::mat4 _view = glm::mat4(1.0F);
        mutable glm::mat4 _projection = glm::mat4(1.0F);
        mutable bool _view_needs_regen = true;
        mutable bool _projection_needs_regen = true;
        float _znear = 0.1F, _zfar = 100.0F;
    };

    class orthographic_camera final : public camera
    {
    public:
        constexpr orthographic_camera() = default;

        constexpr orthographic_camera(
            const float znear,
            const float zfar,
            const glm::vec3 init_pos = { 0, 0, 0 },
            const glm::vec3 init_rot = { 0, 0, 0 }) noexcept
            : camera(init_pos, init_rot, znear, zfar)
        {
        }

        const glm::mat4& get_projection_matrix() const override;
    };

    class perspective_camera final : public camera
    {
    public:
        constexpr perspective_camera() = default;

        constexpr perspective_camera(
            const float vertical_fov,
            const float aspect_ratio,
            const float znear,
            const float zfar,
            const glm::vec3 init_pos = { 0, 0, 0 },
            const glm::vec3 init_rot = { 0, 0, 0 }) noexcept
            : camera(init_pos, init_rot, znear, zfar)
            , _vfov(vertical_fov)
            , _aspect_ratio(aspect_ratio)
        {
        }

        const glm::mat4& get_projection_matrix() const override;

        float vertical_fov() const { return _vfov; }

        float aspect_ratio() const { return _aspect_ratio; }

        void set_aspect_ratio(float ratio);
        void set_vertical_fov(float fov);

    private:
        float _vfov = 60;
        float _aspect_ratio = 1.0F;
    };
} // namespace cathedral::engine
