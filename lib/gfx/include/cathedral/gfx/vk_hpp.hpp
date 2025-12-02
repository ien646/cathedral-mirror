#pragma once

// https://github.com/KhronosGroup/Vulkan-Hpp/issues/2264
#ifdef CATHEDRAL_LINUX_PLATFORM_X11
    #define VULKAN_HPP_ASSERT_ON_RESULT(expression) ((void)0)
#endif

#include <vulkan/vulkan.hpp>

#if VK_HEADER_VERSION >= 330
    #warning "This check is no longer necessary as of SDK version 1.330.*"
#endif