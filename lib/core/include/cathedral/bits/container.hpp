#pragma once

#include <ien/alloc.hpp>

namespace cathedral
{
    template <typename T, size_t Alignment>
    using aligned_vector = std::vector<T, ien::aligned_allocator<T, Alignment>>;
}