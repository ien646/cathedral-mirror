#include <cathedral/gfx/buffers/uniform_buffer.hpp>

#include <cathedral/core.hpp>

namespace cathedral::gfx
{
    namespace
    {
        generic_buffer_args get_uniform_buffer_args(size_t sz, const vulkan_context* vkctx)
        {
            generic_buffer_args result;
            result.size = sz;
            result.memory_flags = vk::MemoryPropertyFlagBits::eDeviceLocal;
            result.usage = vk::BufferUsageFlagBits::eUniformBuffer | vk::BufferUsageFlagBits::eTransferDst;
            result.vkctx = vkctx;
            return result;
        }
    } // namespace

    uniform_buffer::uniform_buffer(uniform_buffer_args args)
        : generic_buffer(get_uniform_buffer_args(args.size, args.vkctx))
    {
    }
} // namespace cathedral::gfx