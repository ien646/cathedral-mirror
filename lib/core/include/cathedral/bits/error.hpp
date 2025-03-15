#pragma once

#include <string>

namespace cathedral
{
    [[noreturn]] void die(const std::string& message, int code = -1);
}

#ifdef CATHEDRAL_NO_CHECKS
    #define CRITICAL_CHECK(cond) ((void)0)
    #define CRITICAL_CHECK_NOTNULL(p) ((void)0)
    #define CRITICAL_ERROR(msg) die("Critical error!")
    #define NOT_IMPLEMENTED() die("Not implemented!")
#else
    #include <format>
    #include <ien/platform.hpp>
    #include <source_location>

    #ifdef IEN_COMPILER_MSVC
        #define CATHEDRAL_CRITICAL_BEGIN_                                                                                   \
            __pragma(warning(push));                                                                                        \
            __pragma(warning(disable : 4702));
        #define CATHEDRAL_CRITICAL_END_ __pragma(warning(pop));
    #else
        #define CATHEDRAL_CRITICAL_BEGIN_
        #define CATHEDRAL_CRITICAL_END_
    #endif

    #ifdef CATHEDRAL_NO_TRACING
        #define CRITICAL_CHECK(cond)                                                                                        \
            CATHEDRAL_CRITICAL_BEGIN_                                                                                       \
            if (!(cond))                                                                                                    \
            {                                                                                                               \
                cathedral::die("Critical check failed!");                                                                   \
            }                                                                                                               \
            CATHEDRAL_CRITICAL_END_
        #define CRITICAL_CHECK_NOTNULL(p)                                                                                   \
            CATHEDRAL_CRITICAL_BEGIN_                                                                                       \
            if (p == nullptr)                                                                                               \
            {                                                                                                               \
                cathedral::die("Critical null check failed!");                                                              \
            }                                                                                                               \
            CATHEDRAL_CRITICAL_END_
        #define CRITICAL_ERROR(msg)                                                                                         \
            CATHEDRAL_CRITICAL_BEGIN_                                                                                       \
            die(msg)                                                                                                        \
            CATHEDRAL_CRITICAL_END_
    #else
        #define CRITICAL_CHECK(cond)                                                                                         \
            CATHEDRAL_CRITICAL_BEGIN_                                                                                        \
            if (!(cond))                                                                                                     \
            {                                                                                                                \
                const auto sloc = std::source_location::current();                                                           \
                cathedral::die(std::format("Critical check failed! At '{}:{}' ({})", sloc.file_name(), sloc.line(), #cond)); \
            }                                                                                                                \
            CATHEDRAL_CRITICAL_END_

        #define CRITICAL_CHECK_NOTNULL(p)                                                                                   \
            CATHEDRAL_CRITICAL_BEGIN_                                                                                       \
            if (p == nullptr)                                                                                               \
            {                                                                                                               \
                const auto sloc = std::source_location::current();                                                          \
                cathedral::die(                                                                                             \
                    std::format("Critical null check failed! At '{}:{}' ({})", sloc.file_name(), sloc.line(), #p));         \
            }                                                                                                               \
            CATHEDRAL_CRITICAL_END_

        #define CRITICAL_ERROR(msg)                                                                                         \
            CATHEDRAL_CRITICAL_BEGIN_                                                                                       \
            {                                                                                                               \
                const auto sloc = std::source_location::current();                                                          \
                cathedral::die(std::format("Critical error! At '{}:{}' ({})", sloc.file_name(), sloc.line(), msg));         \
            }                                                                                                               \
            CATHEDRAL_CRITICAL_END_

        #define NOT_IMPLEMENTED()                                                                                           \
            CATHEDRAL_CRITICAL_BEGIN_                                                                                       \
            {                                                                                                               \
                const auto sloc = std::source_location::current();                                                          \
                cathedral::die(std::format("Not implemented! At {}:{}", sloc.file_name(), sloc.line()));                     \
            }                                                                                                               \
            CATHEDRAL_CRITICAL_END_
    #endif
#endif