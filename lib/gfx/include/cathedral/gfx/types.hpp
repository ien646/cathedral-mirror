#pragma once

#include <cathedral/core.hpp>

#include <vector>

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

    struct vertex_input_description
    {
        uint32_t vertex_size;
        std::vector<vertex_input_attribute> attributes;

        constexpr vertex_input_description() {}

        vertex_input_description(uint32_t vertex_size, const std::vector<vertex_input_attribute>& attributes)
            : vertex_size(vertex_size)
            , attributes(attributes)
        {
        }
    };
}