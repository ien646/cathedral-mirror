#include <cathedral/engine/screen_ray.hpp>

namespace cathedral::engine
{
    ray get_screen_point_camera_ray(const camera& cam, const glm::vec2 normalized_coord)
    {
        const auto& proj_matrix = cam.get_projection_matrix();
        const auto& view_matrix = cam.get_view_matrix();

        const glm::vec4 vs_coord = glm::inverse(proj_matrix) * glm::vec4(normalized_coord, 0, 0);
        const glm::vec4 ws_coord = glm::inverse(view_matrix) * vs_coord;

        const glm::vec3 dir = glm::normalize(glm::vec3(ws_coord));

        return { cam.position(), dir };
    }
} // namespace cathedral::engine