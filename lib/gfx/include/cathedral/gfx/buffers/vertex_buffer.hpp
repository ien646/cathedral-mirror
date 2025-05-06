#pragma once

#include <cathedral/gfx/buffers/generic_buffer.hpp>
#include <cathedral/gfx/vma_forward.hpp>
#include <cathedral/gfx/vulkan_context.hpp>

namespace cathedral::gfx
{
    struct vertex_buffer_args
    {
        const vulkan_context* vkctx = nullptr;
        uint32_t vertex_size = 0;
        size_t size = 0;
    };

    class vertex_buffer : public generic_buffer
    {
    public:
        vertex_buffer(const vertex_buffer_args&);

        inline uint32_t vertex_size() const { return _vertex_size; }

        inline uint32_t vertex_count() const { return static_cast<uint32_t>(_args.size / _vertex_size); }

    private:
        uint32_t _vertex_size;
    };
} // namespace cathedral::gfx