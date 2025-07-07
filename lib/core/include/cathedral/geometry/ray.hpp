#pragma once

#include <cathedral/core.hpp>

#include <glm/vec3.hpp>

FORWARD_CLASS(cathedral, aabb);
FORWARD_CLASS(cathedral, sphere);

namespace cathedral
{
    enum class ray_sphere_intersection_result : uint8_t
    {
        OUTSIDE,
        TANGENT,
        INSIDE
    };

    enum class ray_aabb_intersection_result : uint8_t
    {
        OUTSIDE,
        INSIDE
    };

    struct ray
    {
        glm::vec3 origin;
        glm::vec3 direction;

        ray_sphere_intersection_result sphere_intersection(const sphere&) const;
        ray_sphere_intersection_result sphere_intersection(const sphere&, out_param<float> out_distance) const;

        ray_aabb_intersection_result aabb_intersection(const aabb& box) const;
        ray_aabb_intersection_result aabb_intersection(const aabb& box, float ray_distance) const;
    };
}