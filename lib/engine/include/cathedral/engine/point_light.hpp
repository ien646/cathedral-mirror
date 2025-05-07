#pragma once

#include <cathedral/gfx/aligned_uniform.hpp>

#include <glm/vec3.hpp>

namespace cathedral::engine
{
    struct point_light_data
    {        
        CATHEDRAL_ALIGNED_UNIFORM(glm::vec3, position);
        CATHEDRAL_PADDING_32; 
        CATHEDRAL_ALIGNED_UNIFORM(glm::vec3, color);
        CATHEDRAL_PADDING_32;
        CATHEDRAL_ALIGNED_UNIFORM(float , intensity);
        CATHEDRAL_ALIGNED_UNIFORM(float, range);
        CATHEDRAL_ALIGNED_UNIFORM(float, falloff_coefficient);
        CATHEDRAL_ALIGNED_UNIFORM(bool, enabled);
    };
}