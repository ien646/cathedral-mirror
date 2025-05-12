#pragma once

#include <cathedral/sphere.hpp>

#include <glm/vec3.hpp>
#include <glm/vec4.hpp>

namespace cathedral
{
    enum class plane_point_side
    {
        BEHIND,
        FRONT,
        INTERSECT
    };

    enum class plane_sphere_side
    {
        BEHIND,
        BEHIND_INTERSECT,
        BEHIND_TANGENT,
        CENTER,
        FRONT_TANGENT,
        FRONT_INTERSECT,
        FRONT
    };

    struct plane
    {
        glm::vec3 normal = {};
        float distance = 0.0F;

        constexpr plane() = default;

        constexpr plane(const glm::vec3 normal, const float distance)
            : normal(normal)
            , distance(distance)
        {
        }

        constexpr plane(const float nx, const float ny, const float nz, const float d)
            : normal(nx, ny, nz)
            , distance(d)
        {
        }

        constexpr plane& operator=(const glm::vec4 abcd)
        {
            normal = glm::vec3(abcd);
            distance = abcd.w;
            return *this;
        }

        glm::vec4 as_vec4() const { return glm::vec4(normal.x, normal.y, normal.z, distance); }

        plane_point_side get_side_for_point(glm::vec3 point) const;

        plane_sphere_side get_side_for_sphere(const sphere& s) const;
    };
} // namespace cathedral