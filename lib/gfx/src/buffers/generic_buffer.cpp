#include <cathedral/gfx/buffers/generic_buffer.hpp>

#include <cathedral/core.hpp>

#include <vk_mem_alloc.h>

namespace cathedral::gfx
{
    generic_buffer::generic_buffer(const generic_buffer_args& args)
        : _args(args)
    {
        const uint32_t queue_index = _args.vkctx->graphics_queue_family_index();
        auto buffer_info = zero_struct<VkBufferCreateInfo>();
        buffer_info.pQueueFamilyIndices = &queue_index;
        buffer_info.queueFamilyIndexCount = 1;
        buffer_info.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
        buffer_info.size = _args.size;
        buffer_info.usage = (VkBufferUsageFlags)_args.usage;
        buffer_info.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;

        VmaAllocationCreateInfo alloc_info = {};
        alloc_info.pool = VK_NULL_HANDLE;
        alloc_info.requiredFlags = (VkMemoryPropertyFlags)_args.memory_flags;

        _allocation = new VmaAllocation;
        _allocation_info = new VmaAllocationInfo;

        const auto buffer_create_result =
            vmaCreateBuffer(_args.vkctx->allocator(), &buffer_info, &alloc_info, &_buffer, _allocation, _allocation_info);

        CRITICAL_CHECK(buffer_create_result == VK_SUCCESS, "Failure creating buffer");
    }

    generic_buffer::generic_buffer(generic_buffer&& mv_src) noexcept
        : _args(std::move(mv_src._args))
        , _buffer(mv_src._buffer)
        , _allocation(mv_src._allocation)
        , _allocation_info(mv_src._allocation_info) 
    {
        mv_src._buffer = VK_NULL_HANDLE;
        mv_src._allocation = nullptr;
        mv_src._allocation_info = nullptr;
        mv_src._args = {};
    }

    generic_buffer::~generic_buffer()
    {
        if (_allocation != nullptr)
        {
            vmaDestroyBuffer(_args.vkctx->allocator(), _buffer, *_allocation);
            delete _allocation;
            delete _allocation_info;
            _allocation = nullptr;
            _allocation_info = nullptr;
            _buffer = VK_NULL_HANDLE;
        }
    }
} // namespace cathedral::gfx