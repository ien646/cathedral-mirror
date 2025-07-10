#include <cathedral/engine/frustum.hpp>

#include <cathedral/engine/camera.hpp>
#include <cathedral/geometry/plane.hpp>
#include <cathedral/geometry/sphere.hpp>

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/normal.hpp>

namespace cathedral::engine
{
    frustum_planes get_frustum_from_viewproj_matrix(const glm::mat4& vp_matrix)
    {
        frustum_planes frustum;

        // Left
        frustum.left = glm::vec4(
            vp_matrix[0][3] + vp_matrix[0][0],
            vp_matrix[1][3] + vp_matrix[1][0],
            vp_matrix[2][3] + vp_matrix[2][0],
            vp_matrix[3][3] + vp_matrix[3][0]);

        // Right
        frustum.right = glm::vec4(
            vp_matrix[0][3] - vp_matrix[0][0],
            vp_matrix[1][3] - vp_matrix[1][0],
            vp_matrix[2][3] - vp_matrix[2][0],
            vp_matrix[3][3] - vp_matrix[3][0]);

        // Bottom
        frustum.bottom = glm::vec4(
            vp_matrix[0][3] + vp_matrix[0][1],
            vp_matrix[1][3] + vp_matrix[1][1],
            vp_matrix[2][3] + vp_matrix[2][1],
            vp_matrix[3][3] + vp_matrix[3][1]);

        // Top
        frustum.top = glm::vec4(
            vp_matrix[0][3] - vp_matrix[0][1],
            vp_matrix[1][3] - vp_matrix[1][1],
            vp_matrix[2][3] - vp_matrix[2][1],
            vp_matrix[3][3] - vp_matrix[3][1]);

        // Near
        frustum.near = glm::vec4(
            vp_matrix[0][3] + vp_matrix[0][2],
            vp_matrix[1][3] + vp_matrix[1][2],
            vp_matrix[2][3] + vp_matrix[2][2],
            vp_matrix[3][3] + vp_matrix[3][2]);

        // Far
        frustum.far = glm::vec4(
            vp_matrix[0][3] - vp_matrix[0][2],
            vp_matrix[1][3] - vp_matrix[1][2],
            vp_matrix[2][3] - vp_matrix[2][2],
            vp_matrix[3][3] - vp_matrix[3][2]);

        const auto normalize_plane = [](plane& plane) { plane.abcd /= glm::length(glm::vec3(plane.abcd)); };

        normalize_plane(frustum.near);
        normalize_plane(frustum.far);
        normalize_plane(frustum.left);
        normalize_plane(frustum.right);
        normalize_plane(frustum.bottom);
        normalize_plane(frustum.top);

        return frustum;
    }

    frustum_planes get_frustum_from_camera(const camera& camera)
    {
        const auto vp_matrix = camera.get_projection_matrix() * camera.get_view_matrix();
        return get_frustum_from_viewproj_matrix(vp_matrix);
    }

    bool is_point_inside_frustum(const glm::vec3 point, const frustum_planes& frustum, const bool include_tangent)
    {
        const auto check_plane = [&](const auto& plane) -> bool {
            const auto side = plane.get_side_for_point(point);
            return side == plane_point_side::FRONT || (!(include_tangent) && side == plane_point_side::INTERSECT);
        };

        // Planes are checked in somewhat order of importance for most common situations
        return check_plane(frustum.near) && check_plane(frustum.left) && check_plane(frustum.right) &&
               check_plane(frustum.top) && check_plane(frustum.bottom) && check_plane(frustum.far);
    }

    bool is_aabb_inside_frustum(const aabb& aabb, const frustum_planes& frustum)
    {
        const auto aabb_inside_plane = [&](const plane& plane) -> bool {
            const glm::vec3 pvert = { plane.abcd.x >= 0 ? aabb.max.x : aabb.min.x,
                                      plane.abcd.y >= 0 ? aabb.max.y : aabb.min.y,
                                      plane.abcd.z >= 0 ? aabb.max.z : aabb.min.z };
            const float distance = glm::dot(glm::vec3(plane.abcd), pvert) + plane.abcd.w;
            return distance >= 0;
        };

        return aabb_inside_plane(frustum.near) && aabb_inside_plane(frustum.far) && aabb_inside_plane(frustum.top) ||
               aabb_inside_plane(frustum.bottom) && aabb_inside_plane(frustum.left) && aabb_inside_plane(frustum.right);
    }

    bool is_sphere_inside_frustum(const sphere& sphere, const frustum_planes& frustum)
    {
        const auto sphere_inside_plane = [&](const plane& plane) -> bool {
            const float dist = glm::dot(glm::vec3(plane.abcd), sphere.center) + plane.abcd.w;
            return dist >= -sphere.radius;
        };

        return sphere_inside_plane(frustum.near) && sphere_inside_plane(frustum.far) && sphere_inside_plane(frustum.top) &&
               sphere_inside_plane(frustum.bottom) && sphere_inside_plane(frustum.left) &&
               sphere_inside_plane(frustum.right);
    }
} // namespace cathedral::engine