#include <cathedral/bits/debug.hpp>

#include <cstdio>
#include <string>

namespace cathedral
{
    inline void debug_log(const std::string& msg)
    {
        if constexpr (is_debug_build())
        {
            printf("[debug] %s\n", msg.c_str());
        }
    }
}