#include <cathedral/gfx/buffers/index_buffer.hpp>

#include <cathedral/core.hpp>

namespace cathedral::gfx
{
    inline generic_buffer_args get_index_buffer_args(size_t sz, const vulkan_context* vkctx)
    {
        generic_buffer_args result;
        result.size = sz;
        result.memory_flags = vk::MemoryPropertyFlagBits::eDeviceLocal;
        result.usage = vk::BufferUsageFlagBits::eIndexBuffer | vk::BufferUsageFlagBits::eTransferDst;
        result.vkctx = vkctx;
        return result;
    }

    index_buffer::index_buffer(index_buffer_args args)
        : generic_buffer(get_index_buffer_args(args.size, args.vkctx))
    {
    }
} // namespace cathedral::gfx