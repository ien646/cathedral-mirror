#pragma once

#include <cathedral/core.hpp>

namespace cathedral::gfx
{
    enum class descriptor_type
    {
        UNIFORM,
        STORAGE,
        SAMPLER
    };

    enum class shader_type
    {
        VERTEX,
        FRAGMENT
    };

    enum class vertex_data_type
    {
        FLOAT,
        VEC2F,
        VEC3F,
        VEC4F
    };

    struct vertex_input_attribute
    {
        uint32_t location;
        uint32_t offset;
        vertex_data_type type;
    };
}