#pragma once

#include <glm/vec3.hpp>

namespace cathedral
{
    enum class sphere_contains_point_result
    {
        OUTSIDE,
        TANGENT,
        INSIDE
    };

    enum class sphere_contains_sphere_result
    {
        OUTSIDE,
        TANGENT_OUTSIDE,
        INTERSECT,
        TANGENT_INSIDE,
        INSIDE
    };

    struct sphere
    {
        glm::vec3 center = {};
        float radius = 0.0F;

        constexpr sphere() = default;

        constexpr sphere(const glm::vec3 c, const float r)
            : center(c)
            , radius(r)
        {
        }

        constexpr sphere(const float cx, const float cy, const float cz, const float radius)
            : center(cx, cy, cz)
            , radius(radius)
        {
        }

        sphere_contains_point_result contains_point(glm::vec3 point) const;

        sphere_contains_sphere_result contains_sphere(const sphere& other) const;
    };
} // namespace cathedral