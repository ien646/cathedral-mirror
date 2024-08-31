#include <ien/platform.hpp>

#if defined(IEN_COMPILER_GNU) | defined(IEN_COMPILER_CLANG) | defined(IEN_COMPILER_INTEL)
    #define PRIVATE_SYMBOL __attribute__((visibility("hidden")))
#else
    #define PRIVATE_SYMBOL
#endif