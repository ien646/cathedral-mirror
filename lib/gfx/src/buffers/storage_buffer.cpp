#include <cathedral/gfx/buffers/storage_buffer.hpp>

#include <cathedral/core.hpp>

namespace cathedral::gfx
{
    namespace
    {
        generic_buffer_args get_storage_buffer_args(const size_t sz, const vulkan_context* vkctx)
        {
            generic_buffer_args result;
            result.size = sz;
            result.memory_flags = vk::MemoryPropertyFlagBits::eDeviceLocal;
            result.usage = vk::BufferUsageFlagBits::eStorageBuffer | vk::BufferUsageFlagBits::eTransferDst;
            result.vkctx = vkctx;
            return result;
        }
    } // namespace

    storage_buffer::storage_buffer(const storage_buffer_args args)
        : generic_buffer(get_storage_buffer_args(args.size, args.vkctx))
    {
    }
} // namespace cathedral::gfx