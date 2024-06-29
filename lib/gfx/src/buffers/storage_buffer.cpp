#include <cathedral/gfx/buffers/storage_buffer.hpp>

#include <cathedral/core.hpp>

#include <vk_mem_alloc.h>

namespace cathedral::gfx
{
    inline generic_buffer_args get_storage_buffer_args(size_t sz, const vulkan_context* vkctx)
    {
        generic_buffer_args result;
        result.size = sz;
        result.memory_flags = vk::MemoryPropertyFlagBits::eDeviceLocal;
        result.usage = vk::BufferUsageFlagBits::eStorageBuffer | vk::BufferUsageFlagBits::eTransferDst;
        result.vkctx = vkctx;
        return result;
    }

    storage_buffer::storage_buffer(storage_buffer_args args)
        : generic_buffer(get_storage_buffer_args(args.size, args.vkctx))
    {
    }
} // namespace cathedral::gfx