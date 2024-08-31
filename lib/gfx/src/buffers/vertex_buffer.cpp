#include <cathedral/gfx/buffers/vertex_buffer.hpp>

#include <cathedral/core.hpp>

namespace cathedral::gfx
{
    inline generic_buffer_args get_vertex_buffer_args(size_t sz, const vulkan_context* vkctx)
    {
        generic_buffer_args result;
        result.size = sz;
        result.memory_flags = vk::MemoryPropertyFlagBits::eDeviceLocal;
        result.usage = vk::BufferUsageFlagBits::eVertexBuffer | vk::BufferUsageFlagBits::eTransferDst;
        result.vkctx = vkctx;
        return result;
    }

    vertex_buffer::vertex_buffer(vertex_buffer_args args)
        : generic_buffer(get_vertex_buffer_args(args.size, args.vkctx))
        , _vertex_size(args.vertex_size)
    {
        CRITICAL_CHECK(args.vertex_size > 0);
    }
} // namespace cathedral::gfx