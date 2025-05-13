#include <cathedral/engine/frustum.hpp>

#include <cathedral/plane.hpp>

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/normal.hpp>

namespace cathedral::engine
{
    struct frustum_planes
    {
        plane left, right, bottom, top, near, far;
    };

    frustum_planes get_frustum_planes_from_projection_matrix(const glm::mat4& p)
    {
        frustum_planes result = {};

        // Gribb/Hartmann
        // clang-format off
        result.left = {
            p[0][3] + p[0][0], p[1][3] + p[1][0],  p[2][3] + p[2][0], p[3][3] + p[3][0] };
        result.right = {
            p[0][3] - p[0][0], p[1][3] - p[1][0],  p[2][3] - p[2][0], p[3][3] - p[3][0] };
        result.bottom = {
            p[0][3] + p[0][1], p[1][3] + p[1][1],  p[2][3] + p[2][1], p[3][3] + p[3][1] };
        result.top = {
            p[0][3] - p[0][1], p[1][3] - p[1][1],  p[2][3] - p[2][1], p[3][3] - p[3][1] };
        result.near = {
            p[0][2], p[1][2],  p[2][2], p[3][2] };
        result.far = {
            p[0][3] - p[0][2], p[1][3] - p[1][2],  p[2][3] - p[2][2], p[3][3] - p[3][2] };
        // clang-format on

        return result;
    }

    frustum_normals get_frustum_from_camera(const perspective_camera& camera)
    {
        using triangle = std::array<glm::vec3, 3>;
        frustum_normals result = {};
        const auto projection = camera.get_projection_matrix();

        return result;
    }
} // namespace cathedral::engine