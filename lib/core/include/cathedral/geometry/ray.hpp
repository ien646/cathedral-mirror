#pragma once

#include <cathedral/core.hpp>

#include <glm/vec3.hpp>

FORWARD_CLASS(cathedral, sphere);

namespace cathedral
{
    enum class ray_sphere_intersection_result : uint8_t
    {
        OUTSIDE,
        TANGENT,
        INSIDE
    };

    struct ray
    {
        glm::vec3 origin;
        glm::vec3 direction;

        ray_sphere_intersection_result sphere_intersection(const sphere&) const;
        ray_sphere_intersection_result sphere_intersection(const sphere&, out_param<float> out_distance) const;
    };
}