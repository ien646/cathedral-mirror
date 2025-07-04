#pragma once

#include <memory_resource>
#include <vector>

namespace cathedral
{
    namespace internal
    {
        std::pmr::monotonic_buffer_resource& get_scratch_memory_resource();

        template <typename T>
        std::pmr::polymorphic_allocator<T> get_scratch_allocator()
        {
            return std::pmr::polymorphic_allocator<T>(&get_scratch_memory_resource());
        }
    } // namespace internal

    void init_scratch_memory();
    void reset_scratch_memory();

    template <typename T>
    std::pmr::vector<T> get_scratch_vector()
    {
        return std::pmr::vector<T>(internal::get_scratch_allocator<T>());
    }
} // namespace cathedral