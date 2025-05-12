#include <cathedral/sphere.hpp>

#include <glm/geometric.hpp>

namespace cathedral
{
    sphere_contains_point_result sphere::contains_point(const glm::vec3 point) const
    {
        const float distance = glm::length(point - center);
        if (distance < radius)
        {
            return sphere_contains_point_result::INSIDE;
        }
        if (distance == radius)
        {
            return sphere_contains_point_result::TANGENT;
        }
        // if (distance > radius)
        return sphere_contains_point_result::OUTSIDE;
    }

    sphere_contains_sphere_result sphere::contains_sphere(const sphere& other) const
    {
        const float center_distance = glm::length(center - other.center);
        const float v = center_distance - radius - other.radius;
        if (v == 0)
        {
            return sphere_contains_sphere_result::TANGENT_OUTSIDE;
        }
        if (v > 0)
        {
            return sphere_contains_sphere_result::OUTSIDE;
        }
        if (v == other.radius)
        {
            return sphere_contains_sphere_result::TANGENT_INSIDE;
        }
        if (v > other.radius)
        {
            return sphere_contains_sphere_result::INTERSECT;
        }
        return sphere_contains_sphere_result::INSIDE;
    }
} // namespace cathedral