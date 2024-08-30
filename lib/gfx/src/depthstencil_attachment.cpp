#include <cathedral/gfx/depthstencil_attachment.hpp>

#include <cathedral/core.hpp>

#include <vk_mem_alloc.h>

namespace cathedral::gfx
{
    depthstencil_attachment::depthstencil_attachment(depthstencil_attachment_args args)
        : _args(std::move(args))
    {
        load();
    }

    depthstencil_attachment::~depthstencil_attachment()
    {
        unload();
    }

    void depthstencil_attachment::reload(depthstencil_attachment_args args)
    {
        unload();
        _args = std::move(args);
        load();
    }

    void depthstencil_attachment::unload()
    {
        if (_image != VK_NULL_HANDLE)
        {
            vmaDestroyImage(_args.vkctx->allocator(), _image, *_image_allocation);
            delete _image_allocation;
            delete _image_allocation_info;
            _image = VK_NULL_HANDLE;
            _args = {};
        }
    }

    void depthstencil_attachment::load()
    {
        CRITICAL_CHECK_NOTNULL(_args.vkctx);
        CRITICAL_CHECK(_args.width > 0);
        CRITICAL_CHECK(_args.height > 0);

        auto gfx_queue_index = _args.vkctx->graphics_queue_family_index();

        auto info = zero_struct<VkImageCreateInfo>();
        info.arrayLayers = 1;
        info.extent = vk::Extent3D(_args.width, _args.height, 1);
        info.format = VK_FORMAT_D32_SFLOAT_S8_UINT;
        info.imageType = VK_IMAGE_TYPE_2D;
        info.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
        info.mipLevels = 1;
        info.pQueueFamilyIndices = &gfx_queue_index;
        info.queueFamilyIndexCount = 1;
        info.samples = VK_SAMPLE_COUNT_1_BIT;
        info.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
        info.tiling = VK_IMAGE_TILING_OPTIMAL;
        info.usage = VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT | VK_IMAGE_USAGE_SAMPLED_BIT;
        info.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;

        auto alloc_info = zero_struct<VmaAllocationCreateInfo>();
        alloc_info.usage = VMA_MEMORY_USAGE_GPU_ONLY;

        _image_allocation = new VmaAllocation;
        _image_allocation_info = new VmaAllocationInfo;

        auto image_create_result =
            vmaCreateImage(_args.vkctx->allocator(), &info, &alloc_info, &_image, _image_allocation, _image_allocation_info);
        CRITICAL_CHECK(image_create_result == VK_SUCCESS);

        vk::ImageViewCreateInfo depth_imageview_info;
        depth_imageview_info.components = vk::ComponentMapping();
        depth_imageview_info.format = static_cast<vk::Format>(info.format);
        depth_imageview_info.image = _image;
        depth_imageview_info.subresourceRange.aspectMask = vk::ImageAspectFlagBits::eDepth;
        depth_imageview_info.subresourceRange.baseArrayLayer = 0;
        depth_imageview_info.subresourceRange.baseMipLevel = 0;
        depth_imageview_info.subresourceRange.layerCount = 1;
        depth_imageview_info.subresourceRange.levelCount = 1;
        depth_imageview_info.viewType = vk::ImageViewType::e2D;

        vk::ImageViewCreateInfo stencil_imageview_info = depth_imageview_info;
        stencil_imageview_info.subresourceRange.aspectMask = vk::ImageAspectFlagBits::eStencil;

        vk::ImageViewCreateInfo depth_stencil_imageviewinfo = depth_imageview_info;
        depth_stencil_imageviewinfo.subresourceRange.aspectMask |= vk::ImageAspectFlagBits::eStencil;

        _depth_imageview = _args.vkctx->device().createImageViewUnique(depth_imageview_info);
        _stencil_imageview = _args.vkctx->device().createImageViewUnique(stencil_imageview_info);
        _depthstencil_imageview = _args.vkctx->device().createImageViewUnique(depth_stencil_imageviewinfo);

        auto cmdbuff = _args.vkctx->create_primary_commandbuffer();
        cmdbuff->begin(vk::CommandBufferBeginInfo{});
        {
            vk::ImageMemoryBarrier2 transition_ds_barrier;
            transition_ds_barrier.dstAccessMask = vk::AccessFlagBits2::eMemoryRead | vk::AccessFlagBits2::eMemoryWrite;
            transition_ds_barrier.srcAccessMask = vk::AccessFlagBits2::eNone;
            transition_ds_barrier.dstStageMask = vk::PipelineStageFlagBits2::eAllCommands;
            transition_ds_barrier.srcStageMask = vk::PipelineStageFlagBits2::eNone;
            transition_ds_barrier.image = _image;
            transition_ds_barrier.oldLayout = vk::ImageLayout::eUndefined;
            transition_ds_barrier.newLayout = vk::ImageLayout::eDepthStencilAttachmentOptimal;
            transition_ds_barrier.subresourceRange.aspectMask = vk::ImageAspectFlagBits::eDepth |
                                                                vk::ImageAspectFlagBits::eStencil;
            transition_ds_barrier.subresourceRange.baseArrayLayer = 0;
            transition_ds_barrier.subresourceRange.baseMipLevel = 0;
            transition_ds_barrier.subresourceRange.layerCount = 1;
            transition_ds_barrier.subresourceRange.levelCount = 1;

            vk::DependencyInfoKHR dependency_info;
            dependency_info.bufferMemoryBarrierCount = 0;
            dependency_info.memoryBarrierCount = 0;
            dependency_info.imageMemoryBarrierCount = 1;
            dependency_info.pImageMemoryBarriers = &transition_ds_barrier;
            cmdbuff->pipelineBarrier2(dependency_info);
        }
        cmdbuff->end();
        _args.vkctx->submit_commandbuffer_sync(*cmdbuff);
    }
} // namespace cathedral::gfx