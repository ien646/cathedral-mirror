#include <cathedral/engine/frustum.hpp>

#include <cathedral/plane.hpp>

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

#define CATHEDRAL_FRUSTUM_CHECK_PLANE(plane, point, include_tangent)                                                        \
    if (const auto side = plane.get_side_for_point(point);                                                                  \
        side == plane_point_side::BEHIND || (!include_tangent && side == plane_point_side::INTERSECT))                      \
    {                                                                                                                       \
        return false;                                                                                                       \
    }

    bool is_point_inside_frustum(const glm::vec3 point, const frustum_planes& frustum, const bool include_tangent)
    {
        // Planes are checked in somewhat order of importance for most common situations
        CATHEDRAL_FRUSTUM_CHECK_PLANE(frustum.near, point, include_tangent);
        CATHEDRAL_FRUSTUM_CHECK_PLANE(frustum.left, point, include_tangent);
        CATHEDRAL_FRUSTUM_CHECK_PLANE(frustum.right, point, include_tangent);
        CATHEDRAL_FRUSTUM_CHECK_PLANE(frustum.top, point, include_tangent);
        CATHEDRAL_FRUSTUM_CHECK_PLANE(frustum.bottom, point, include_tangent);
        CATHEDRAL_FRUSTUM_CHECK_PLANE(frustum.far, point, include_tangent);

        return true;
    }
} // namespace cathedral::engine