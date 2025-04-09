#pragma once

#include <cathedral/core.hpp>

#include <cstdint>

namespace cathedral::engine
{
    enum class shader_material_uniform_binding : uint8_t
    {
        MATERIAL_ID
    };

    enum class shader_node_uniform_binding : uint8_t
    {
        NODE_ID,
        NODE_MODEL_MATRIX
    };

    using shader_uniform_binding = std::variant<shader_material_uniform_binding, shader_node_uniform_binding>;

    enum class shader_texture_binding : uint8_t
    {
        PLACEHOLDER
    };

    constexpr uint32_t sizeof_shader_material_uniform_binding(const shader_material_uniform_binding binding)
    {
        using enum shader_material_uniform_binding;
        switch (binding)
        {
        case MATERIAL_ID:
            return sizeof(uint32_t);
        }
        CRITICAL_ERROR("Unhandled shader material uniform binding")
    }

    constexpr uint32_t sizeof_shader_node_uniform_binding(const shader_node_uniform_binding binding)
    {
        using enum shader_node_uniform_binding;
        switch (binding)
        {
        case NODE_ID:
            return sizeof(uint32_t);
        case NODE_MODEL_MATRIX:
            return 4 * 4 * sizeof(float);
        }
        CRITICAL_ERROR("Unhandled shader node uniform binding");
    }
} // namespace cathedral::engine