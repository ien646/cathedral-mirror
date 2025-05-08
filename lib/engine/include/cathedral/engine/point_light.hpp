#pragma once

#include <cathedral/gfx/aligned_uniform.hpp>

#include <glm/vec3.hpp>

namespace cathedral::engine
{
    struct point_light_data
    {
        CATHEDRAL_ALIGNED_UNIFORM(glm::vec3, position) = { 0, 0, 0 };
        CATHEDRAL_PADDING_32;
        CATHEDRAL_ALIGNED_UNIFORM(glm::vec3, color) = { 1, 1, 1 };
        CATHEDRAL_PADDING_32;
        CATHEDRAL_ALIGNED_UNIFORM(float, intensity) = { 1 };
        CATHEDRAL_ALIGNED_UNIFORM(float, range) = { 1 };
        CATHEDRAL_ALIGNED_UNIFORM(float, falloff_coefficient) = { 1 };
        CATHEDRAL_ALIGNED_UNIFORM(bool, enabled) = true;
    };
} // namespace cathedral::engine