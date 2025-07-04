#include <cathedral/memory.hpp>

#include <cathedral/core.hpp>

#include <memory>
#include <vector>

#ifndef CATHEDRAL_SCRATCH_BUFFER_SIZE
    #define CATHEDRAL_SCRATCH_BUFFER_SIZE 1024 * 1024 * 128
#endif

namespace cathedral
{
    namespace internal
    {
        std::vector<std::byte> scratch_buffer;
        std::unique_ptr<std::pmr::monotonic_buffer_resource> scratch_resource;

        std::pmr::monotonic_buffer_resource& get_scratch_memory_resource()
        {
            CRITICAL_CHECK(scratch_resource != nullptr, "Scratch memory not initialized!");
            return *scratch_resource;
        }
    } // namespace internal

    void init_scratch_memory()
    {
        internal::scratch_buffer.resize(CATHEDRAL_SCRATCH_BUFFER_SIZE);
        internal::scratch_resource = std::make_unique<std::pmr::monotonic_buffer_resource>(
            internal::scratch_buffer.data(),
            internal::scratch_buffer.size());
    }

    void reset_scratch_memory()
    {
        internal::scratch_resource = std::make_unique<std::pmr::monotonic_buffer_resource>(
            internal::scratch_buffer.data(),
            internal::scratch_buffer.size());
    }
} // namespace cathedral