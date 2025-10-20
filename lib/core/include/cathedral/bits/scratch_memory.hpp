#pragma once

#include <memory_resource>
#include <vector>

namespace cathedral
{
    namespace internal
    {
        class tracking_monotonic_buffer_resource final : public std::pmr::memory_resource
        {
        public:
            tracking_monotonic_buffer_resource(void* buffer, const std::size_t buffer_size)
                : _resource(buffer, buffer_size)
                , _buffer_ptr(buffer)
            {
            }

            void release() { _resource.release(); }

            size_t allocated_memory() const { return _allocated; }

        private:
            std::pmr::monotonic_buffer_resource _resource;
            void* _buffer_ptr;
            size_t _allocated = 0;

            void* do_allocate(const std::size_t bytes, const std::size_t alignment) override
            {
                void* ptr = _resource.allocate(bytes, alignment);
                _allocated = (static_cast<uint8_t*>(ptr) - static_cast<uint8_t*>(_buffer_ptr)) + bytes;
                return ptr;
            }

            void do_deallocate(
                [[maybe_unused]] void* ptr,
                [[maybe_unused]] std::size_t bytes,
                [[maybe_unused]] std::size_t alignment) override
            {
                // noop
            }

            bool do_is_equal(const memory_resource& other) const noexcept override { return this == &other; }
        };

        tracking_monotonic_buffer_resource& get_scratch_memory_resource();

        template <typename T>
        std::pmr::polymorphic_allocator<T> get_scratch_allocator()
        {
            return std::pmr::polymorphic_allocator<T>(&get_scratch_memory_resource());
        }
    } // namespace internal

    void init_scratch_memory();
    void flush_scratch_memory();
    size_t scratch_memory_usage();

    template <typename T>
    std::pmr::vector<T> get_scratch_vector()
    {
        return std::pmr::vector<T>(internal::get_scratch_allocator<T>());
    }
} // namespace cathedral