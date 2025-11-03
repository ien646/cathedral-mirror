#pragma once

#include <cathedral/core.hpp>

// Side effects guaranteed
#define CATHEDRAL_VK_RESULT_VALUE_CHECKED(x)                                                                                \
    [&] CATHEDRAL_FORCE_INLINE {                                                                                            \
        auto _vulkan_result_checked_varname_ = x;                                                                           \
        CRITICAL_CHECK(                                                                                                     \
            _vulkan_result_checked_varname_.result == vk::Result::eSuccess,                                                 \
            std::format("Vulkan result check failure: {}", #x));                                                            \
        return std::move(_vulkan_result_checked_varname_.value);                                                            \
    }()

// Side effects guaranteed
#define CATHEDRAL_VK_RESULT_CHECKED(x)                                                                                      \
    {                                                                                                                       \
        if ((x) != vk::Result::eSuccess)                                                                                    \
        {                                                                                                                   \
            CRITICAL_ERROR(std::format("Vulkan result check failure: {}", #x));                                             \
        }                                                                                                                   \
    }