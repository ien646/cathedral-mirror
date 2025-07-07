#include <cathedral/engine/frustum.hpp>

#include <cathedral/geometry/plane.hpp>
#include <cathedral/geometry/sphere.hpp>

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/normal.hpp>

namespace cathedral::engine
{
    frustum_planes get_frustum_planes_from_projection_matrix(const glm::mat4& p)
    {
        frustum_planes result = {};

        // Gribb/Hartmann
        // clang-format off
        result.left = plane{
            p[0][3] + p[0][0], p[1][3] + p[1][0],  p[2][3] + p[2][0], p[3][3] + p[3][0] };
        result.right = plane{
            p[0][3] - p[0][0], p[1][3] - p[1][0],  p[2][3] - p[2][0], p[3][3] - p[3][0] };
        result.bottom = plane{
            p[0][3] + p[0][1], p[1][3] + p[1][1],  p[2][3] + p[2][1], p[3][3] + p[3][1] };
        result.top = plane{
            p[0][3] - p[0][1], p[1][3] - p[1][1],  p[2][3] - p[2][1], p[3][3] - p[3][1] };
        result.near = plane{
            p[0][2], p[1][2],  p[2][2], p[3][2] };
        result.far = plane{
            p[0][3] - p[0][2], p[1][3] - p[1][2],  p[2][3] - p[2][2], p[3][3] - p[3][2] };
        // clang-format on

        return result;
    }

    frustum_planes get_frustum_from_camera(const perspective_camera& camera)
    {
        auto result = get_frustum_planes_from_projection_matrix(camera.get_projection_matrix());

        const auto& view = camera.get_view_matrix();

        result.left = view * result.left.as_vec4();
        result.right = view * result.right.as_vec4();
        result.bottom = view * result.bottom.as_vec4();
        result.top = view * result.top.as_vec4();
        result.near = view * result.near.as_vec4();
        result.far = view * result.far.as_vec4();

        return result;
    }

    bool is_point_inside_frustum(const glm::vec3 point, const frustum_planes& frustum, const bool include_tangent)
    {
        const auto check_plane = [&](const auto& plane) -> bool {
            const auto side = plane.get_side_for_point(point);
            return side == plane_point_side::BEHIND || (!(include_tangent) && side == plane_point_side::INTERSECT);
        };

        // Planes are checked in somewhat order of importance for most common situations
        return check_plane(frustum.near) || check_plane(frustum.left) || check_plane(frustum.right) ||
               check_plane(frustum.top) || check_plane(frustum.bottom) || check_plane(frustum.far);
    }

    bool is_aabb_inside_frustum(const aabb& aabb, const frustum_planes& frustum)
    {
        const auto aabb_outside_plane = [&](const plane& plane) -> bool {
            const glm::vec4 pv4 = plane.as_vec4();
            return (glm::dot(pv4, glm::vec4(aabb.min.x, aabb.min.y, aabb.min.z, 1.0F)) < 0.0F) &&
                   (glm::dot(pv4, glm::vec4(aabb.min.x, aabb.max.y, aabb.min.z, 1.0F)) < 0.0F) &&
                   (glm::dot(pv4, glm::vec4(aabb.min.x, aabb.min.y, aabb.max.z, 1.0F)) < 0.0F) &&
                   (glm::dot(pv4, glm::vec4(aabb.min.x, aabb.max.y, aabb.max.z, 1.0F)) < 0.0F) &&
                   (glm::dot(pv4, glm::vec4(aabb.max.x, aabb.min.y, aabb.max.z, 1.0F)) < 0.0F) &&
                   (glm::dot(pv4, glm::vec4(aabb.max.x, aabb.min.y, aabb.min.z, 1.0F)) < 0.0F) &&
                   (glm::dot(pv4, glm::vec4(aabb.max.x, aabb.max.y, aabb.min.z, 1.0F)) < 0.0F) &&
                   (glm::dot(pv4, glm::vec4(aabb.max.x, aabb.max.y, aabb.max.z, 1.0F)) < 0.0F);
        };

        return aabb_outside_plane(frustum.near) && aabb_outside_plane(frustum.far) && aabb_outside_plane(frustum.top) &&
               aabb_outside_plane(frustum.bottom) && aabb_outside_plane(frustum.left) && aabb_outside_plane(frustum.right);
    }

    bool is_sphere_inside_frustum(const sphere& sphere, const frustum_planes& frustum)
    {
        const auto sphere_outside_plane = [&](const plane& plane) -> bool {
            const glm::vec3 m = (plane.normal * sphere.center);
            return (m.x + m.y + m.z + plane.distance) > sphere.radius;
        };

        return sphere_outside_plane(frustum.near) && sphere_outside_plane(frustum.far) &&
               sphere_outside_plane(frustum.top) && sphere_outside_plane(frustum.bottom) &&
               sphere_outside_plane(frustum.left) && sphere_outside_plane(frustum.right);
    }
} // namespace cathedral::engine