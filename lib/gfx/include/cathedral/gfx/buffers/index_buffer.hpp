#pragma once

#include <cathedral/gfx/buffers/generic_buffer.hpp>
#include <cathedral/gfx/vma_forward.hpp>
#include <cathedral/gfx/vulkan_context.hpp>

namespace cathedral::gfx
{
    struct index_buffer_args
    {
        const vulkan_context* vkctx = nullptr;
        size_t size = 0;
    };

    class index_buffer : public generic_buffer
    {
    public:
        index_buffer(index_buffer_args);

        inline uint32_t index_count() const { return _args.size / sizeof(uint32_t); }
    };
} // namespace cathedral::gfx