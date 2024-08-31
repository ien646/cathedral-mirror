#pragma once

#include <cathedral/gfx/buffers/generic_buffer.hpp>
#include <cathedral/gfx/vma_forward.hpp>
#include <cathedral/gfx/vulkan_context.hpp>

namespace cathedral::gfx
{
    struct staging_buffer_args
    {
        const vulkan_context* vkctx = nullptr;
        size_t size = 0;
    };

    class staging_buffer : public generic_buffer
    {
    public:
        staging_buffer(staging_buffer_args);

        void* map_memory();
        void unmap_memory();

    private:
        void* _mapped_memory = nullptr;
    };
} // namespace cathedral::gfx