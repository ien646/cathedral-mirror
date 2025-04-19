#pragma once

#include <string>

namespace cathedral
{
    [[noreturn]] void die(const std::string& message, int code = -1);
}

#ifdef CATHEDRAL_NO_CHECKS
    #define CRITICAL_CHECK(cond, msg) ((void)0)
    #define CRITICAL_CHECK_NOTNULL(p) ((void)0)
    #define CRITICAL_ERROR(msg) die("Critical error!")
    #define NOT_IMPLEMENTED() die("Not implemented!")
#else
    #include <format>
    #include <ien/platform.hpp>
    #include <source_location>

    #ifdef CATHEDRAL_NO_TRACING
        #define CRITICAL_CHECK(cond, msg)                                                                                   \
            {                                                                                                               \
                if (!(cond))                                                                                                \
                {                                                                                                           \
                    cathedral::die(std::format("Critical check failed! => {}", msg));                                       \
                }                                                                                                           \
            }

        #define CRITICAL_CHECK_NOTNULL(p)                                                                                   \
            {                                                                                                               \
                if (p == nullptr)                                                                                           \
                {                                                                                                           \
                    cathedral::die("Critical null check failed!");                                                          \
                }                                                                                                           \
            }

        #define CRITICAL_ERROR(msg) die(msg)

    #else
        #define CRITICAL_CHECK(cond, msg)                                                                                   \
            {                                                                                                               \
                if (!(cond))                                                                                                \
                {                                                                                                           \
                    const auto sloc = std::source_location::current();                                                      \
                    cathedral::die(std::format(                                                                             \
                        "Critical check failed! At '{}:{}::{}' ({}) => {}",                                                 \
                        sloc.file_name(),                                                                                   \
                        sloc.line(),                                                                                        \
                        sloc.function_name(),                                                                               \
                        #cond,                                                                                              \
                        msg));                                                                                              \
                }                                                                                                           \
            }

        #define CRITICAL_CHECK_NOTNULL(p)                                                                                   \
            {                                                                                                               \
                if (p == nullptr)                                                                                           \
                {                                                                                                           \
                    const auto sloc = std::source_location::current();                                                      \
                    cathedral::die(std::format(                                                                             \
                        "Critical null check failed! At '{}:{}::{}' ({})",                                                  \
                        sloc.file_name(),                                                                                   \
                        sloc.line(),                                                                                        \
                        sloc.function_name(),                                                                               \
                        #p));                                                                                               \
                }                                                                                                           \
            }

        #define CRITICAL_ERROR(msg)                                                                                         \
            {                                                                                                               \
                const auto sloc = std::source_location::current();                                                          \
                cathedral::die(std::format(                                                                                 \
                    "Critical error! At '{}:{}::{}' ({})",                                                                  \
                    sloc.file_name(),                                                                                       \
                    sloc.line(),                                                                                            \
                    sloc.function_name(),                                                                                   \
                    msg));                                                                                                  \
            }

        #define NOT_IMPLEMENTED()                                                                                           \
            {                                                                                                               \
                const auto sloc = std::source_location::current();                                                          \
                cathedral::die(                                                                                             \
                    std::format("Not implemented! At {}:{}::{}", sloc.file_name(), sloc.line(), sloc.function_name()));     \
            }

    #endif
#endif