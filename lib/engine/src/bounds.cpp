#include <cathedral/engine/bounds.hpp>

namespace cathedral::engine
{
    bool bounds_collection::ray_intersects(const ray& r) const
    {
        for (const auto& figure : figures)
        {
            if (std::holds_alternative<sphere>(figure))
            {
                return r.sphere_intersection(std::get<sphere>(figure)) == ray_sphere_intersection_result::INSIDE;
            }
            if (std::holds_alternative<aabb>(figure))
            {
                return r.aabb_intersection(std::get<aabb>(figure)) == ray_aabb_intersection_result::INSIDE;
            }
        }
        return false;
    }

    bool bounds_collection::inside_frustum(const frustum_planes& frustum) const
    {
        for (const auto& figure : figures)
        {
            if (std::holds_alternative<sphere>(figure))
            {
                return is_sphere_inside_frustum(std::get<sphere>(figure), frustum);
            }
            if (std::holds_alternative<aabb>(figure))
            {
                return is_aabb_inside_frustum(std::get<aabb>(figure), frustum);
            }
        }
        return false;
    }
} // namespace cathedral::engine