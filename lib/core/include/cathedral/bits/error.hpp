#pragma once

#include <format>
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
    #ifdef CATHEDRAL_NO_TRACING
        #define CRITICAL_CHECK(cond)                                                                                        \
            if (!(cond))                                                                                                    \
            {                                                                                                               \
                cathedral::die("Critical check failed!");                                                                   \
            }
        #define CRITICAL_CHECK_NOTNULL(p)                                                                                   \
            if (p == nullptr)                                                                                               \
            {                                                                                                               \
                cathedral::die("Critical null check failed!");                                                              \
            }
        #define CRITICAL_ERROR(msg) die(msg)
    #else
        #define CRITICAL_CHECK(cond)                                                                                        \
            if (!(cond))                                                                                                    \
            {                                                                                                               \
                cathedral::die(std::format("Critical check failed! At '{}:{}' ({})", __FILE__, __LINE__, #cond));           \
            }
        #define CRITICAL_CHECK_NOTNULL(p)                                                                                   \
            if (p == nullptr)                                                                                               \
            {                                                                                                               \
                cathedral::die(std::format("Critical null check failed! At '{}:{}' ({})", __FILE__, __LINE__, #p));         \
            }
        #define CRITICAL_ERROR(msg) cathedral::die(std::format("Critical error! At '{}:{}' ({})", __FILE__, __LINE__, msg))

        #define NOT_IMPLEMENTED() cathedral::die(std::format("Not implemented! At {}:{}", __FILE__, __LINE__))
    #endif
#endif