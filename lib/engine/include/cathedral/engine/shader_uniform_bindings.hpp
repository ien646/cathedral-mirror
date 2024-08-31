#pragma once

#include <cstdint>

namespace cathedral::engine
{
    enum class shader_uniform_binding
    {
        NODE_ID,
        NODE_MODEL_MATRIX
    };

    constexpr uint32_t sizeof_shader_uniform_binding(shader_uniform_binding binding)
    {
        using enum shader_uniform_binding;
        switch (binding)
        {
        case NODE_ID:
            return sizeof(uint32_t);
        case NODE_MODEL_MATRIX:
            return 4 * 4 * sizeof(float);
        }
    }
} // namespace cathedral::engine