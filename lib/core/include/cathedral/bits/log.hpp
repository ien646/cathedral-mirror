#pragma once

#include <iostream>
#include <print>

namespace cathedral
{
#ifndef NDEBUG
    inline void debug_log(const std::string& msg)
    {
        std::println(std::cout, "[cathedral-debug] {}", msg);
    }

    inline void debug_log_error(const std::string& msg)
    {
        std::println(std::cerr, "[cathedral-debug] {}", msg);
    }
#else
    inline void debug_log(const std::string& msg)
    {
    }

    inline void debug_log_error(const std::string& msg)
    {
    }
#endif

    inline void log(const std::string& msg)
    {
        std::println(std::cout, "[cathedral] {}", msg);
    }

    inline void log_error(const std::string& msg)
    {
        std::println(std::cerr, "[cathedral] {}", msg);
    }

} // namespace cathedral