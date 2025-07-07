#include <cathedral/geometry/ray.hpp>

#include <cathedral/geometry/aabb.hpp>
#include <cathedral/geometry/sphere.hpp>

#include <glm/geometric.hpp>

#include <algorithm>

#include <glm/vec2.hpp>

namespace cathedral
{
    ray_sphere_intersection_result ray::sphere_intersection(const sphere& s) const
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

    namespace
    {
        std::pair<float, float> slab_intersection_get_near_far(const ray& ray, const aabb& box)
        {
            const glm::vec3 tmin = (box.min - ray.origin) / ray.direction;
            const glm::vec3 tmax = (box.max - ray.origin) / ray.direction;

            const glm::vec3 tnear = glm::min(tmin, tmax);
            const glm::vec3 tfar = glm::max(tmin, tmax);

            const float near = std::max({ tnear.x, tnear.y, tnear.z });
            const float far = std::min({ tfar.x, tfar.y, tfar.z });

            return { near, far };
        }
    } // namespace

    ray_aabb_intersection_result ray::aabb_intersection(const aabb& box) const
    {
        const auto [near, far] = slab_intersection_get_near_far(*this, box);
        if (near < 0.0F || near >= far)
        {
            return ray_aabb_intersection_result::OUTSIDE;
        }
        return ray_aabb_intersection_result::INSIDE;
    }

    ray_aabb_intersection_result ray::aabb_intersection(const aabb& box, float ray_distance) const
    {
        const auto [near, far] = slab_intersection_get_near_far(*this, box);
        if (near < 0.0F || near >= far || near > ray_distance)
        {
            return ray_aabb_intersection_result::OUTSIDE;
        }
        return ray_aabb_intersection_result::INSIDE;
    }
} // namespace cathedral