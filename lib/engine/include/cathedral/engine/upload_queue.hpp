#pragma once

#include <cathedral/gfx/buffers.hpp>
#include <cathedral/gfx/image.hpp>
#include <cathedral/gfx/vulkan_context.hpp>

namespace cathedral::engine
{
    class upload_queue
    {
    public:
        upload_queue(const gfx::vulkan_context& vkctx, uint32_t staging_buff_size);

        void update_buffer(const gfx::uniform_buffer& target_buffer, uint32_t target_offset, const void* source, uint32_t size);
        void update_buffer(const gfx::storage_buffer& target_buffer, uint32_t target_offset, const void* source, uint32_t size);
        void update_buffer(const gfx::vertex_buffer& target_buffer, uint32_t target_offset, const void* source, uint32_t size);

        void update_image(const gfx::image& target_image, const void* source, uint32_t size, uint32_t mip_level);

        void record(std::function<void(vk::CommandBuffer)> fn);

        void ready_for_submit();

        inline const gfx::vulkan_context& vkctx() const { return _vkctx; }

        inline vk::CommandBuffer get_cmdbuff() const { return *_cmdbuff; }

    private:
        const gfx::vulkan_context& _vkctx;
        std::unique_ptr<gfx::staging_buffer> _staging_buffer;
        vk::UniqueCommandBuffer _cmdbuff;
        bool _recording = false;

        uint32_t _offset = 0;

        void update_generic_buffer(
            const gfx::generic_buffer& target_buffer,
            uint32_t target_offset,
            const void* source,
            uint32_t size);

        void submit_current();
    };
} // namespace cathedral::engine