#pragma once

#include <glm/vec3.hpp>

namespace cathedral
{
    struct aabb
    {
        glm::vec3 min;
        glm::vec3 max;
    };
}