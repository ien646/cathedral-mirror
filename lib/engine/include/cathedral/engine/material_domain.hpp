#pragma once

#include <cstdint>

namespace cathedral::engine
{
    enum class material_domain : uint8_t
    {
        OPAQUE,
        TRANSPARENT,
        OVERLAY
    };
} // namespace cathedral::engine