#include <cathedral/geometry/aabb.hpp>

namespace cathedral
{
    aabb make_aabb_from_vertices(const std::vector<glm::vec3>& vertices, float padding)
    {
        float min_x = std::numeric_limits<float>::max();
        float min_y = std::numeric_limits<float>::max();
        float min_z = std::numeric_limits<float>::max();
        float max_x = std::numeric_limits<float>::lowest();
        float max_y = std::numeric_limits<float>::lowest();
        float max_z = std::numeric_limits<float>::lowest();

        for (size_t i = 0; i < vertices.size(); i++)
        {
            min_x = std::min(vertices[i].x, min_x);
            min_y = std::min(vertices[i].y, min_y);
            min_z = std::min(vertices[i].z, min_z);
            max_x = std::max(vertices[i].x, max_x);
            max_y = std::max(vertices[i].y, max_y);
            max_z = std::max(vertices[i].z, max_z);
        }

        min_x -= padding;
        min_y -= padding;
        min_z -= padding;
        max_x += padding;
        max_y += padding;
        max_z += padding;

        return aabb{ { min_x - padding, min_y - padding, min_z - padding },
                     { max_x + padding, max_y + padding, max_z + padding } };
    }
} // namespace cathedral