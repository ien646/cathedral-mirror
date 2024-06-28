#include <cathedral/gfx/image.hpp>

namespace cathedral::gfx
{
    image::image(image_args args)
        : _vkctx(args.vkctx)
        , _width(args.width)
        , _height(args.height)
        , _aspect_flags(args.aspect_flags)
        , _mip_levels(args.mipmap_levels)
        , _layout(vk::ImageLayout::eUndefined)
        , _format(args.format)
    {
        CRITICAL_CHECK(args.validate());

        const auto gfx_family_index = args.vkctx->graphics_queue_family_index();

        vk::ImageCreateInfo image_info;
        image_info.imageType = vk::ImageType::e2D;
        image_info.arrayLayers = 1;
        image_info.extent = vk::Extent3D(args.width, args.height, 1.0f);
        image_info.format = args.format;
        image_info.initialLayout = vk::ImageLayout::eUndefined;
        image_info.pQueueFamilyIndices = &gfx_family_index;
        image_info.queueFamilyIndexCount = 1;
        image_info.mipLevels = args.mipmap_levels;
        image_info.samples = vk::SampleCountFlagBits::e1;
        image_info.sharingMode = vk::SharingMode::eExclusive;
        image_info.tiling = vk::ImageTiling::eOptimal;
        image_info.usage = vk::ImageUsageFlagBits::eSampled | vk::ImageUsageFlagBits::eTransferDst | vk::ImageUsageFlagBits::eTransferSrc;

        VmaAllocationCreateInfo alloc_info = zero_struct<VmaAllocationCreateInfo>();
        alloc_info.usage = VMA_MEMORY_USAGE_GPU_ONLY;

        VkImageCreateInfo& vk_image_info = image_info;

        auto result = vmaCreateImage(_vkctx->allocator(), &vk_image_info, &alloc_info, &_image, &_allocation, &_allocation_info);
        CRITICAL_CHECK(result == VK_SUCCESS);

        vk::ImageViewCreateInfo imgview_info;
        imgview_info.image = _image;
        imgview_info.viewType = vk::ImageViewType::e2D;
        imgview_info.components.r = vk::ComponentSwizzle::eIdentity;
        imgview_info.components.g = vk::ComponentSwizzle::eIdentity;
        imgview_info.components.b = vk::ComponentSwizzle::eIdentity;
        imgview_info.components.a = vk::ComponentSwizzle::eIdentity;
        imgview_info.format = _format;
        imgview_info.subresourceRange.aspectMask = _aspect_flags;
        imgview_info.subresourceRange.baseArrayLayer = 0;
        imgview_info.subresourceRange.baseMipLevel = 0;
        imgview_info.subresourceRange.layerCount = 1;
        imgview_info.subresourceRange.levelCount = _mip_levels;
        
        _imageview = _vkctx->device().createImageViewUnique(imgview_info);
    }

    image::~image()
    {
        vmaDestroyImage(_vkctx->allocator(), _image, _allocation);
    }

    void image::transition_layout(
        vk::ImageLayout layout,
        vk::CommandBuffer cmdbuff,
        vk::ImageAspectFlags aspect,
        uint32_t first_mip,
        uint32_t mip_count)
    {
        vk::ImageMemoryBarrier2 barrier;
        barrier.image = _image;
        barrier.oldLayout = _layout;
        barrier.newLayout = layout;
        barrier.srcAccessMask = vk::AccessFlagBits2::eMemoryRead | vk::AccessFlagBits2::eMemoryWrite;
        barrier.srcStageMask = vk::PipelineStageFlagBits2::eAllCommands;
        barrier.srcQueueFamilyIndex = _vkctx->graphics_queue_family_index();
        barrier.dstAccessMask = vk::AccessFlagBits2::eMemoryRead | vk::AccessFlagBits2::eMemoryWrite;
        barrier.dstStageMask = vk::PipelineStageFlagBits2::eAllCommands;
        barrier.dstQueueFamilyIndex = _vkctx->graphics_queue_family_index();
        barrier.subresourceRange.aspectMask = aspect;
        barrier.subresourceRange.baseArrayLayer = 0;
        barrier.subresourceRange.baseMipLevel = first_mip;
        barrier.subresourceRange.layerCount = 1;
        barrier.subresourceRange.levelCount = mip_count;

        vk::DependencyInfo depinfo;
        depinfo.imageMemoryBarrierCount = 1;
        depinfo.pImageMemoryBarriers = &barrier;

        cmdbuff.pipelineBarrier2(depinfo);

        _layout = layout;
    }
} // namespace zynr::gfx