#include <cathedral/plane.hpp>

#include <cathedral/sphere.hpp>

#include <cmath>

namespace cathedral
{
    plane_point_side plane::get_side_for_point(const glm::vec3 point) const
    {
        const float v = normal.x * point.x + normal.y * point.y + normal.z * point.z + distance;
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
        const float v = (normal.x * s.center.x + normal.y * s.center.y + normal.z * s.center.z + distance) /
                        std::sqrt(normal.x * normal.x + normal.y * normal.y + normal.z * normal.z);

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
        // if (abs_v < sphere_radius) // intersect
        return v < 0.0F ? plane_sphere_side::BEHIND_INTERSECT : plane_sphere_side::FRONT_INTERSECT;
    }
} // namespace cathedral