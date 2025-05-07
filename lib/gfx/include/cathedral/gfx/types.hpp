#pragma once

#include <cathedral/core.hpp>

#include <limits>
#include <vector>

namespace cathedral::gfx
{
    enum class descriptor_type : uint8_t
    {
        UNDEFINED,
        UNIFORM,
        STORAGE,
        SAMPLER
    };

    enum class shader_type : uint8_t
    {
        UNDEFINED,
        VERTEX,
        FRAGMENT
    };

    enum class vertex_data_type : uint8_t
    {
        UNDEFINED,
        FLOAT,
        VEC2F,
        VEC3F,
        VEC4F
    };

    struct vertex_input_attribute
    {
        uint32_t location = std::numeric_limits<uint32_t>::max();
        uint32_t offset = std::numeric_limits<uint32_t>::max();
        vertex_data_type type = vertex_data_type::UNDEFINED;
    };

    struct vertex_input_description
    {
        uint32_t vertex_size = 0;
        std::vector<vertex_input_attribute> attributes;

        constexpr vertex_input_description() = default;

        vertex_input_description(const uint32_t vertex_size, const std::vector<vertex_input_attribute>& attributes)
            : vertex_size(vertex_size)
            , attributes(attributes)
        {
        }
    };
} // namespace cathedral::gfx