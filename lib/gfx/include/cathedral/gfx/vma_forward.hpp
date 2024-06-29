#pragma once

#include <vulkan/vulkan_core.h>

struct VmaAllocation_T;
typedef VmaAllocation_T* VmaAllocation;

struct VmaAllocator_T;
typedef VmaAllocator_T* VmaAllocator;

#ifndef VMA_NULLABLE
    #ifdef __clang__
        #define VMA_NULLABLE _Nullable
    #else
        #define VMA_NULLABLE
    #endif
#endif

#ifndef VMA_NULLABLE_NON_DISPATCHABLE
    #if defined(__LP64__) || defined(_WIN64) || (defined(__x86_64__) && !defined(__ILP32__) ) || defined(_M_X64) || defined(__ia64) || defined (_M_IA64) || defined(__aarch64__) || defined(__powerpc64__)
        #define VMA_NULLABLE_NON_DISPATCHABLE VMA_NULLABLE
    #else
        #define VMA_NULLABLE_NON_DISPATCHABLE
    #endif
#endif

struct VmaAllocationInfo;