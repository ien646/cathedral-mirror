#pragma once

#include <cathedral/gfx/vma_forward.hpp>
#include <cathedral/gfx/vulkan_context.hpp>

namespace cathedral::gfx
{
    struct generic_buffer_args
    {
        const vulkan_context* vkctx = nullptr;
        size_t size = 0;
        vk::BufferUsageFlags usage;
        vk::MemoryPropertyFlags memory_flags;
    };

    class generic_buffer
    {
    public:
        generic_buffer(const generic_buffer_args&);
        generic_buffer(const generic_buffer&) = delete;
        generic_buffer(generic_buffer&&) noexcept;
        virtual ~generic_buffer();

        inline vk::Buffer buffer() const { return _buffer; }

        inline size_t size() const { return _args.size; }

    protected:
        generic_buffer_args _args;
        VkBuffer _buffer = VK_NULL_HANDLE;
        VmaAllocation* _allocation = nullptr;
        VmaAllocationInfo* _allocation_info = nullptr;
    };
} // namespace cathedral::gfx