#include <cathedral/engine/bounds.hpp>

namespace cathedral::engine
{
    bool node_bounds::ray_intersects(const ray& r) const
    {
        for (const auto& figure : figures)
        {
            if (std::holds_alternative<sphere>(figure))
            {
                if (r.sphere_intersection(std::get<sphere>(figure)) == ray_sphere_intersection_result::INSIDE)
                {
                    return true;
                }
            }
            else if (std::holds_alternative<aabb>(figure))
            {
                if (r.aabb_intersection(std::get<aabb>(figure)) == ray_aabb_intersection_result::INSIDE)
                {
                    return true;
                }
            }
        }
        return false;
    }

    bool node_bounds::inside_frustum(const frustum_planes& frustum) const
    {
        for (const auto& figure : figures)
        {
            if (std::holds_alternative<sphere>(figure))
            {
                if (is_sphere_inside_frustum(std::get<sphere>(figure), frustum))
                {
                    return true;
                }
            }
            if (std::holds_alternative<aabb>(figure))
            {
                if (is_aabb_inside_frustum(std::get<aabb>(figure), frustum))
                {
                    return true;
                }
            }
        }
        return false;
    }
} // namespace cathedral::engine