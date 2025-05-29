#pragma once

#include <cathedral/bits/error.hpp>

#ifndef NDEBUG
    #define DEBUG_CHECK(cond, msg) CRITICAL_CHECK(cond, msg)
#else
    #define DEBUG_CHECK(cond, msg) ((void)0)
#endif

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