#include <cathedral/engine/upload_queue.hpp>

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
    }

    void upload_queue::
        update_buffer(const gfx::uniform_buffer& target_buffer, uint32_t target_offset, const void* source, uint32_t size)
    {
        update_generic_buffer(target_buffer, target_offset, source, size);
    }

    void upload_queue::
        update_buffer(const gfx::storage_buffer& target_buffer, uint32_t target_offset, const void* source, uint32_t size)
    {
        update_generic_buffer(target_buffer, target_offset, source, size);
    }

    void upload_queue::
        update_buffer(const gfx::vertex_buffer& target_buffer, uint32_t target_offset, const void* source, uint32_t size)
    {
        update_generic_buffer(target_buffer, target_offset, source, size);
    }

    void upload_queue::update_image(const gfx::image& target_image, const void* source, uint32_t size, uint32_t mip_level)
    {
        CRITICAL_CHECK(target_image.current_layout() == vk::ImageLayout::eTransferDstOptimal);

        if (!_recording)
        {
            _cmdbuff->begin(vk::CommandBufferBeginInfo{});
            _recording = true;
        }

        if (size > _staging_buffer->size())
        {
            CRITICAL_ERROR("Image update operation exceeds size of staging buffer");
            return;
        }

        if (_offset + size > _staging_buffer->size())
        {
            submit_current();
        }

        uint8_t* mem = reinterpret_cast<uint8_t*>(_staging_buffer->map_memory());
        std::memcpy(mem + _offset, source, size);

        vk::BufferImageCopy copy;
        copy.bufferImageHeight = 0;
        copy.bufferRowLength = 0;
        copy.bufferOffset = _offset;
        copy.imageOffset = vk::Offset3D{ 0, 0, 0 };
        copy.imageExtent = vk::Extent3D{ target_image.width(), target_image.height(), 1 };
        copy.imageSubresource.aspectMask = target_image.aspect_flags();
        copy.imageSubresource.baseArrayLayer = 0;
        copy.imageSubresource.mipLevel = mip_level;
        copy.imageSubresource.layerCount = 1;

        _cmdbuff->copyBufferToImage(
            _staging_buffer->buffer(),
            target_image.get_image(),
            vk::ImageLayout::eTransferDstOptimal,
            copy);

        _offset += size;
    }

    void upload_queue::record(std::function<void(vk::CommandBuffer)> fn)
    {
        if (!_recording)
        {
            _cmdbuff->begin(vk::CommandBufferBeginInfo{});
            _recording = true;
        }
        fn(*_cmdbuff);
    }

    void upload_queue::ready_for_submit()
    {
        if (!_recording)
        {
            _cmdbuff->begin(vk::CommandBufferBeginInfo{});
        }
        _cmdbuff->end();
        _recording = false;
    }

    void upload_queue::
        update_generic_buffer(const gfx::generic_buffer& target_buffer, uint32_t target_offset, const void* source, uint32_t size)
    {
        if (size > _staging_buffer->size())
        {
            CRITICAL_ERROR("Buffer update operation exceeds size of staging buffer");
            return;
        }

        if (!_recording)
        {
            _cmdbuff->begin(vk::CommandBufferBeginInfo{});
            _recording = true;
        }

        if (_offset + size > _staging_buffer->size())
        {
            submit_current();
        }

        uint8_t* mem = reinterpret_cast<uint8_t*>(_staging_buffer->map_memory());
        std::memcpy(mem + _offset, source, size);

        vk::BufferCopy copy;
        copy.srcOffset = _offset;
        copy.dstOffset = target_offset;
        copy.size = size;

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

        _offset += size;
    }

    void upload_queue::submit_current()
    {
        _cmdbuff->end();
        _vkctx.submit_commandbuffer_sync(*_cmdbuff);

        _cmdbuff->reset();
        _cmdbuff->begin(vk::CommandBufferBeginInfo{});
        _offset = 0;
    }
} // namespace cathedral::engine