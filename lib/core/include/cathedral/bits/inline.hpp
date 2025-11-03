#pragma once

#include <ien/platform.hpp>

#if defined(IEN_COMPILER_GNU)
    #define CATHEDRAL_FORCE_INLINE [[gnu::always_inline]]
#elif defined(IEN_COMPILER_CLANG)
    #define CATHEDRAL_FORCE_INLINE [[clang::always_inline]]
#elif defined(IEN_COMPILER_MSVC)
    #define CATHEDRAL_FORCE_INLINE [[msvc::forceinline]]
#endif