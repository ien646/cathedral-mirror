#include <cathedral/geometry/plane.hpp>

#include <cathedral/geometry/sphere.hpp>

#include <cmath>

namespace cathedral
{
    plane_point_side plane::get_side_for_point(const glm::vec3 point) const
    {
        const float v = (abcd.x * point.x) + (abcd.y * point.y) + (abcd.z * point.z) + abcd.w;
        if (v < 0.0F)
        {
            return plane_point_side::BEHIND;
        }
        if (v > 0.0F)
        {
            return plane_point_side::FRONT;
        }
        return plane_point_side::INTERSECT;
    }

    plane_sphere_side plane::get_side_for_sphere(const sphere& s) const
    {
        const float v = (abcd.x * s.center.x + abcd.y * s.center.y + abcd.z * s.center.z + abcd.w) /
                        std::sqrt((abcd.x * abcd.x) + (abcd.y * abcd.y) + (abcd.z * abcd.z));

        if (v == 0)
        {
            return plane_sphere_side::CENTER;
        }

        const float abs_v = std::abs(v);

        if (abs_v > s.radius) // no intersection
        {
            return v < 0.0F ? plane_sphere_side::BEHIND : plane_sphere_side::FRONT;
        }
        if (abs_v == s.radius) // tangent
        {
            return v < 0.0F ? plane_sphere_side::BEHIND_TANGENT : plane_sphere_side::FRONT_TANGENT;
        }
        return v < 0.0F ? plane_sphere_side::BEHIND_INTERSECT : plane_sphere_side::FRONT_INTERSECT;
    }
} // namespace cathedral