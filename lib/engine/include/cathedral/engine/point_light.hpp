#pragma once

#include <cathedral/gfx/aligned_uniform.hpp>

#include <glm/vec3.hpp>

namespace cathedral::engine
{
    struct alignas(16) point_light_data
    {
        CATHEDRAL_ALIGNED_UNIFORM(bool, enabled);
        CATHEDRAL_ALIGNED_UNIFORM(glm::vec3, position);
        CATHEDRAL_ALIGNED_UNIFORM(glm::vec3, color);
        CATHEDRAL_ALIGNED_UNIFORM(float, range);
        CATHEDRAL_ALIGNED_UNIFORM(float, falloff_coefficient);
    };
}