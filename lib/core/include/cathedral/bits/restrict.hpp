#pragma once

#include <ien/platform.hpp>

#ifdef IEN_COMPILER_MSVC
#define CATHEDRAL_RESTRICT_PTR __restrict
#else
#define CATHEDRAL_RESTRICT_PTR __restrict__
#endif