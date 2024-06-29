#pragma once

#include <cathedral/gfx/buffers/generic_buffer.hpp>
#include <cathedral/gfx/vma_forward.hpp>
#include <cathedral/gfx/vulkan_context.hpp>

namespace cathedral::gfx
{
    struct uniform_buffer_args
    {
        const vulkan_context* vkctx = nullptr;
        size_t size = 0;
    };

    class uniform_buffer
        : public generic_buffer
    {
    public:
        uniform_buffer(uniform_buffer_args);
    };
}