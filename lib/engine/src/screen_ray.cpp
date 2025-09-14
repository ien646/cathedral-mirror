#include <cathedral/engine/screen_ray.hpp>

namespace cathedral::engine
{
    ray get_screen_point_camera_ray(const camera& cam, const glm::vec2 normalized_coord)
    {
        const glm::mat4 inv_proj_view = glm::inverse(cam.get_projection_matrix() * cam.get_view_matrix());

        const glm::vec4 nc = { normalized_coord, 0.0F, 1.0F };
        const glm::vec4 fc = { normalized_coord, 1.0F, 1.0F };

        glm::vec4 nw = inv_proj_view * nc;
        glm::vec4 fw = inv_proj_view * fc;

        nw /= nw.w;
        fw /= fw.w;

        return { .origin = glm::vec3(nw), .direction = glm::normalize(glm::vec3(fw - nw)) };
    }
} // namespace cathedral::engine