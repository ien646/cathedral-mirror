#pragma once

#include <cstdint>

namespace cathedral::engine
{
    enum class render_domain : uint8_t
    {
        OPAQUE,
        TRANSPARENT,
        OVERLAY
    };
}