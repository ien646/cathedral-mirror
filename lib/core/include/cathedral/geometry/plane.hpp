#pragma once

#include <cathedral/geometry/sphere.hpp>

#include <glm/vec3.hpp>
#include <glm/vec4.hpp>

namespace cathedral
{
    enum class plane_point_side : uint8_t
    {
        BEHIND,
        FRONT,
        INTERSECT
    };

    enum class plane_sphere_side : uint8_t
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
        glm::vec4 abcd = {};

        constexpr plane() = default;

        constexpr plane& operator=(const glm::vec4 abcd)
        {
            this->abcd = abcd;
            return *this;
        }

        plane_point_side get_side_for_point(glm::vec3 point) const;

        plane_sphere_side get_side_for_sphere(const sphere& s) const;
    };
} // namespace cathedral