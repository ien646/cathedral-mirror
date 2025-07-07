#pragma once

#include <glm/vec3.hpp>

#include <vector>

namespace cathedral
{
    struct aabb
    {
        glm::vec3 min;
        glm::vec3 max;
    };

    aabb make_aabb_from_vertices(const std::vector<glm::vec3>& vertices, float padding = 0.0F);
}