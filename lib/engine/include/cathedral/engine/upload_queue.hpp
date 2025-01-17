#pragma once

#include <cathedral/gfx/buffers.hpp>
#include <cathedral/gfx/image.hpp>
#include <cathedral/gfx/vulkan_context.hpp>

namespace cathedral::engine
{
    enum class upload_queue_state : uint8_t
    {
        READY_TO_RECORD,
        RECORDING,
        PENDING_SUBMIT,
        SUBMITTED
    };

    class upload_queue
    {
    public:
        upload_queue(const gfx::vulkan_context& vkctx, uint32_t staging_buff_size);

        void update_buffer(const gfx::index_buffer& target_buffer, uint32_t target_offset, std::span<const std::byte> data);
        void update_buffer(const gfx::uniform_buffer& target_buffer, uint32_t target_offset, std::span<const std::byte> data);
        void update_buffer(const gfx::storage_buffer& target_buffer, uint32_t target_offset, std::span<const std::byte> data);
        void update_buffer(const gfx::vertex_buffer& target_buffer, uint32_t target_offset, std::span<const std::byte> data);

        template <typename T>
        void update_buffer(const gfx::index_buffer& target_buffer, uint32_t target_offset, std::span<const T> data)
        {
            update_buffer(
                target_buffer,
                target_offset,
                std::span<const std::byte>(reinterpret_cast<const std::byte*>(data.data()), data.size_bytes()));
        }

        template <typename T>
        void update_buffer(const gfx::uniform_buffer& target_buffer, uint32_t target_offset, std::span<const T> data)
        {
            update_buffer(
                target_buffer,
                target_offset,
                std::span<const std::byte>(reinterpret_cast<const std::byte*>(data.data()), data.size_bytes()));
        }

        template <typename T>
        void update_buffer(const gfx::storage_buffer& target_buffer, uint32_t target_offset, std::span<const T> data)
        {
            update_buffer(
                target_buffer,
                target_offset,
                std::span<const std::byte>(reinterpret_cast<const std::byte*>(data.data()), data.size_bytes()));
        }

        template <typename T>
        void update_buffer(const gfx::vertex_buffer& target_buffer, uint32_t target_offset, std::span<const T> data)
        {
            update_buffer(
                target_buffer,
                target_offset,
                std::span<const std::byte>(reinterpret_cast<const std::byte*>(data.data()), data.size_bytes()));
        }

        void update_image(const gfx::image& target_image, std::span<const std::byte> data, uint32_t mip_level);

        void record(const std::function<void(vk::CommandBuffer)>& fn);

        void prepare_to_submit();
        void notify_submitted();
        void notify_fence_waited();

        const gfx::vulkan_context& vkctx() const { return _vkctx; }

        vk::CommandBuffer get_cmdbuff() const { return *_cmdbuff; }

        vk::Fence get_fence() const { return *_fence; }

        bool fence_needs_waiting() const { return _fence_needs_wait; }

    private:
        const gfx::vulkan_context& _vkctx;
        std::unique_ptr<gfx::staging_buffer> _staging_buffer;
        vk::UniqueCommandBuffer _cmdbuff;
        vk::UniqueFence _fence;
        upload_queue_state _state = upload_queue_state::READY_TO_RECORD;
        bool _fence_needs_wait = true;

        uint32_t _offset = 0;

        void update_generic_buffer(
            const gfx::generic_buffer& target_buffer,
            uint32_t target_offset,
            std::span<const std::byte> data);

        void submit_current();

        void prepare_to_record();
    };
} // namespace cathedral::engine