#pragma once

namespace cathedral
{
    constexpr bool is_debug_build()
    {
#ifndef NDEBUG
        return true;
#else
        return false;
#endif
    }
} // namespace cathedral