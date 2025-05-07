#include <cathedral/gfx/buffers/index_buffer.hpp>

#include <cathedral/core.hpp>

namespace cathedral::gfx
{
    namespace
    {
        generic_buffer_args get_index_buffer_args(const size_t sz, const vulkan_context* vkctx)
        {
            generic_buffer_args result;
            result.size = sz;
            result.memory_flags = vk::MemoryPropertyFlagBits::eDeviceLocal;
            result.usage = vk::BufferUsageFlagBits::eIndexBuffer | vk::BufferUsageFlagBits::eTransferDst;
            result.vkctx = vkctx;
            return result;
        }
    } // namespace

    index_buffer::index_buffer(const index_buffer_args args)
        : generic_buffer(get_index_buffer_args(args.size, args.vkctx))
    {
    }
} // namespace cathedral::gfx