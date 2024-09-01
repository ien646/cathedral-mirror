#include <cathedral/engine/upload_queue.hpp>

#include <cmath>

namespace cathedral::engine
{
    upload_queue::upload_queue(const gfx::vulkan_context& vkctx, uint32_t staging_buff_size)
        : _vkctx(vkctx)
    {
        gfx::staging_buffer_args sbuff_args;
        sbuff_args.size = staging_buff_size;
        sbuff_args.vkctx = &vkctx;

        _staging_buffer = std::make_unique<gfx::staging_buffer>(sbuff_args);

        _cmdbuff = vkctx.create_primary_commandbuffer();

        _fence = vkctx.create_signaled_fence();
    }

    void upload_queue::update_buffer(
        const gfx::index_buffer& target_buffer,
        uint32_t target_offset,
        std::span<const std::byte> data)
    {
        update_generic_buffer(target_buffer, target_offset, data);
    }

    void upload_queue::update_buffer(
        const gfx::uniform_buffer& target_buffer,
        uint32_t target_offset,
        std::span<const std::byte> data)
    {
        update_generic_buffer(target_buffer, target_offset, data);
    }

    void upload_queue::update_buffer(
        const gfx::storage_buffer& target_buffer,
        uint32_t target_offset,
        std::span<const std::byte> data)
    {
        update_generic_buffer(target_buffer, target_offset, data);
    }

    void upload_queue::update_buffer(
        const gfx::vertex_buffer& target_buffer,
        uint32_t target_offset,
        std::span<const std::byte> data)
    {
        update_generic_buffer(target_buffer, target_offset, data);
    }

    void upload_queue::update_image(const gfx::image& target_image, std::span<const std::byte> data, uint32_t mip_level)
    {
        prepare_to_record();

        if (data.size() > _staging_buffer->size())
        {
            CRITICAL_ERROR("Image update operation exceeds size of staging buffer");
            return;
        }

        // Align offset to 16 bytes
        if (_offset % 16 != 0)
        {
            _offset += (16 - (_offset % 16));
        }

        if (_offset + data.size() > _staging_buffer->size())
        {
            submit_current();
        }

        auto* mem = reinterpret_cast<uint8_t*>(_staging_buffer->map_memory());
        std::memcpy(mem + _offset, data.data(), data.size());

        const auto target_width = static_cast<uint32_t>(target_image.width() / std::pow(2, mip_level));
        const auto target_height = static_cast<uint32_t>(target_image.height() / std::pow(2, mip_level));

        if (target_width == 0 || target_height == 0)
        {
            return;
        }

        vk::BufferImageCopy copy;
        copy.bufferImageHeight = 0;
        copy.bufferRowLength = 0;
        copy.bufferOffset = _offset;
        copy.imageOffset = vk::Offset3D{ 0, 0, 0 };
        copy.imageExtent = vk::Extent3D{ target_width, target_height, 1 };
        copy.imageSubresource.aspectMask = target_image.aspect_flags();
        copy.imageSubresource.baseArrayLayer = 0;
        copy.imageSubresource.mipLevel = mip_level;
        copy.imageSubresource.layerCount = 1;

        _cmdbuff->copyBufferToImage(
            _staging_buffer->buffer(),
            target_image.get_image(),
            vk::ImageLayout::eTransferDstOptimal,
            copy);

        _offset += data.size();
    }

    void upload_queue::record(const std::function<void(vk::CommandBuffer)>& fn)
    {
        prepare_to_record();
        fn(*_cmdbuff);
    }

    void upload_queue::prepare_to_submit()
    {
        prepare_to_record();
        _cmdbuff->end();
        _state = upload_queue_state::PENDING_SUBMIT;
    }

    void upload_queue::notify_submitted()
    {
        _state = upload_queue_state::SUBMITTED;
        _fence_needs_wait = true;
    }

    void upload_queue::notify_fence_waited()
    {
        _fence_needs_wait = false;
    }

    void upload_queue::update_generic_buffer(
        const gfx::generic_buffer& target_buffer,
        uint32_t target_offset,
        std::span<const std::byte> data)
    {
        if (data.size() > _staging_buffer->size())
        {
            CRITICAL_ERROR("Buffer update operation exceeds size of staging buffer");
            return;
        }

        prepare_to_record();

        if (_offset + data.size() > _staging_buffer->size())
        {
            submit_current();
        }

        auto* mem = reinterpret_cast<uint8_t*>(_staging_buffer->map_memory());
        std::memcpy(mem + _offset, data.data(), data.size());

        vk::BufferCopy copy;
        copy.srcOffset = _offset;
        copy.dstOffset = target_offset;
        copy.size = data.size();

        _cmdbuff->copyBuffer(_staging_buffer->buffer(), target_buffer.buffer(), copy);

        vk::BufferMemoryBarrier barrier;
        barrier.buffer = target_buffer.buffer();
        barrier.srcAccessMask = vk::AccessFlagBits::eMemoryWrite;
        barrier.dstAccessMask = vk::AccessFlagBits::eMemoryWrite;
        barrier.dstQueueFamilyIndex = _vkctx.graphics_queue_family_index();
        barrier.offset = copy.dstOffset;
        barrier.size = copy.size;

        _cmdbuff->pipelineBarrier(
            vk::PipelineStageFlagBits::eTransfer,
            vk::PipelineStageFlagBits::eTransfer,
            static_cast<vk::DependencyFlags>(0),
            {},
            barrier,
            {});

        _offset += data.size();
    }

    void upload_queue::submit_current()
    {
        _cmdbuff->end();
        _vkctx.submit_commandbuffer_sync(*_cmdbuff);

        _cmdbuff->reset();
        _cmdbuff->begin(vk::CommandBufferBeginInfo{});
        _offset = 0;
        _state = upload_queue_state::RECORDING;
    }

    void upload_queue::prepare_to_record()
    {
        switch (_state)
        {
        case upload_queue_state::READY_TO_RECORD:
            _cmdbuff->begin(vk::CommandBufferBeginInfo{});
            _state = upload_queue_state::RECORDING;
            break;
        case upload_queue_state::RECORDING:
            break;
        case upload_queue_state::PENDING_SUBMIT:
            CRITICAL_ERROR("Attempt to record into pending upload queue");
            break;
        case upload_queue_state::SUBMITTED:
            if (_fence_needs_wait)
            {
                const vk::Result wait_result = _vkctx.device().waitForFences(*_fence, true, UINT64_MAX);
                CRITICAL_CHECK(wait_result == vk::Result::eSuccess);
                _vkctx.device().resetFences(*_fence);
                _fence_needs_wait = false;
            }
            _cmdbuff->reset();
            _cmdbuff->begin(vk::CommandBufferBeginInfo{});
            _state = upload_queue_state::RECORDING;
            break;
        }
    }
} // namespace cathedral::engine