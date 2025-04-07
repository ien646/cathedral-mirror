#pragma once

#include <cathedral/core.hpp>

#include <cstdint>

namespace cathedral::engine
{
    enum class shader_uniform_binding : uint8_t
    {
        NODE_ID,
        NODE_MODEL_MATRIX
    };

    enum class shader_texture_binding : uint8_t
    {
        PLACEHOLDER
    };

    constexpr uint32_t sizeof_shader_uniform_binding(const shader_uniform_binding binding)
    {
        using enum shader_uniform_binding;
        switch (binding)
        {
        case NODE_ID:
            return sizeof(uint32_t);
        case NODE_MODEL_MATRIX:
            return 4 * 4 * sizeof(float);
        }
        CRITICAL_ERROR("Unhandled shader uniform binding");
    }
} // namespace cathedral::engine