#include <cathedral/gfx/image.hpp>

#include <cathedral/gfx/vulkan_context.hpp>

#include <cmath>

namespace cathedral::gfx
{
    image::image(image_args args)
        : _vkctx(args.vkctx)
        , _width(args.width)
        , _height(args.height)
        , _aspect_flags(args.aspect_flags)
        , _format(args.format)
        , _mip_levels(args.mipmap_levels)
    {
        CRITICAL_CHECK(args.validate(), "Invalid vulkan image args");

        // Clamp mip levels
        _mip_levels = std::min(_mip_levels, get_max_mip_levels(_width, _height));

        const auto gfx_family_index = args.vkctx->graphics_queue_family_index();

        vk::ImageCreateInfo image_info;
        image_info.imageType = vk::ImageType::e2D;
        image_info.arrayLayers = 1;
        image_info.extent = vk::Extent3D(_width, _height, 1U);
        image_info.format = args.format;
        image_info.initialLayout = vk::ImageLayout::eUndefined;
        image_info.pQueueFamilyIndices = &gfx_family_index;
        image_info.queueFamilyIndexCount = 1;
        image_info.mipLevels = _mip_levels;
        image_info.samples = vk::SampleCountFlagBits::e1;
        image_info.sharingMode = vk::SharingMode::eExclusive;
        image_info.tiling = args.tiling;
        image_info.usage = args.usage_flags;
        if (args.compressed)
        {
            image_info.flags = vk::ImageCreateFlagBits::eBlockTexelViewCompatible | vk::ImageCreateFlagBits::eMutableFormat |
                               vk::ImageCreateFlagBits::eExtendedUsage;
        }

        auto alloc_info = zero_struct<VmaAllocationCreateInfo>();
        alloc_info.usage = VMA_MEMORY_USAGE_AUTO_PREFER_DEVICE;
        if (args.allow_host_memory_mapping)
        {
            alloc_info.flags = VMA_ALLOCATION_CREATE_HOST_ACCESS_RANDOM_BIT | VMA_ALLOCATION_CREATE_MAPPED_BIT;
        }

        VkImageCreateInfo& vk_image_info = image_info;

        auto result =
            vmaCreateImage(_vkctx->allocator(), &vk_image_info, &alloc_info, &_image, &_allocation, &_allocation_info);
        CRITICAL_CHECK(result == VK_SUCCESS, "Failure creating vulkan (VMA) image");
    }

    image::~image()
    {
        vmaDestroyImage(_vkctx->allocator(), _image, _allocation);
    }

    void image::transition_layout(
        vk::ImageLayout old_layout,
        vk::ImageLayout new_layout,
        vk::CommandBuffer cmdbuff,
        vk::ImageAspectFlags aspect,
        uint32_t first_mip,
        uint32_t mip_count)
    {
        vk::ImageMemoryBarrier2 barrier;
        barrier.image = _image;
        barrier.oldLayout = old_layout;
        barrier.newLayout = new_layout;
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
    }
} // namespace cathedral::gfx