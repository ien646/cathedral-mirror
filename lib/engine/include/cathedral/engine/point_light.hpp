#pragma once

#include <cathedral/gfx/aligned_uniform.hpp>

#include <glm/vec3.hpp>

namespace cathedral::engine
{
    struct point_light_data
    {        
        CATHEDRAL_ALIGNED_UNIFORM(glm::vec3, position);
        float _padding0;
        CATHEDRAL_ALIGNED_UNIFORM(glm::vec3, color);
        float _padding1;
        CATHEDRAL_ALIGNED_UNIFORM(float, range);
        CATHEDRAL_ALIGNED_UNIFORM(float, falloff_coefficient);
        CATHEDRAL_ALIGNED_UNIFORM(bool, enabled);
        float _padding2;
    };
}