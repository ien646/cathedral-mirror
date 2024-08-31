#include <cathedral/gfx/buffers/staging_buffer.hpp>

#include <cathedral/core.hpp>

#include <vk_mem_alloc.h>

namespace cathedral::gfx
{
    inline generic_buffer_args get_staging_buffer_args(size_t sz, const vulkan_context* vkctx)
    {
        generic_buffer_args result;
        result.size = sz;
        result.memory_flags = vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent;
        result.usage = vk::BufferUsageFlagBits::eTransferSrc | vk::BufferUsageFlagBits::eTransferDst;
        result.vkctx = vkctx;
        return result;
    }

    staging_buffer::staging_buffer(staging_buffer_args args)
        : generic_buffer(get_staging_buffer_args(args.size, args.vkctx))
    {
    }

    void* staging_buffer::map_memory()
    {
        if (!_mapped_memory)
        {
            _mapped_memory = _args.vkctx->device().mapMemory(_allocation_info->deviceMemory, 0, _allocation_info->size);
        }
        return _mapped_memory;
    }

    void staging_buffer::unmap_memory()
    {
        if (_mapped_memory)
        {
            _args.vkctx->device().unmapMemory(_allocation_info->deviceMemory);
            _mapped_memory = nullptr;
        }
    }
} // namespace cathedral::gfx