#pragma once

#include <cathedral/core.hpp>

// Side effects guaranteed
#define CATHEDRAL_VK_RESULT_CHECKED(x)                                                                                      \
    [&] CATHEDRAL_FORCE_INLINE {                                                                                            \
        auto _vulkan_result_checked_varname_ = x;                                                                           \
        CRITICAL_CHECK(x.result == vk::Result::eSuccess, std::format("Vulkan result check failure: {}", #x));               \
        return std::move(x.value);                                                                                          \
    }()
