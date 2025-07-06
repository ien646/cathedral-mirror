#include <cathedral/geometry/ray.hpp>

#include <cathedral/geometry/sphere.hpp>
#include <glm/geometric.hpp>

namespace cathedral
{
    ray_sphere_intersection_result ray::sphere_intersection(const sphere& s) const
    {
        const glm::vec3 oc = origin - s.center;
        const float a = glm::dot(direction, direction);
        const float b = 2 * glm::dot(oc, direction);
        const float c = glm::dot(oc, oc) - s.radius * s.radius;
        const float d = b * b - 4 * a * c;

        if (d < 0.0F)
        {
            return ray_sphere_intersection_result::OUTSIDE;
        }
        if (d > 0.0F)
        {
            return ray_sphere_intersection_result::INSIDE;
        }
        return ray_sphere_intersection_result::TANGENT;
    }

    ray_sphere_intersection_result ray::sphere_intersection(const sphere& s, out_param<float> out_distance) const
    {
        const glm::vec3 oc = origin - s.center;
        const float a = glm::dot(direction, direction);
        const float b = 2 * glm::dot(oc, direction);
        const float c = glm::dot(oc, oc) - (s.radius * s.radius);
        const float d = (b * b) - (4 * a * c);

        if (d < 0.0F)
        {
            return ray_sphere_intersection_result::OUTSIDE;
        }

        out_distance = (-b - std::sqrt(d)) / (2.0F * a);
        if (d > 0.0F)
        {
            return ray_sphere_intersection_result::INSIDE;
        }
        return ray_sphere_intersection_result::TANGENT;
    }
} // namespace cathedral