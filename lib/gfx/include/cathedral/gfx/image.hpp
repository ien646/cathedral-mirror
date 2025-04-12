#pragma once

#include <cathedral/core.hpp>

#include <ien/image/image.hpp>

#include <vulkan/vulkan.hpp>

#include <vk_mem_alloc.h>

#include <cmath>

namespace cathedral::gfx
{
    FORWARD_CLASS_INLINE(vulkan_context);

    struct image_args
    {
        const vulkan_context* vkctx = nullptr;
        uint32_t width = 0;
        uint32_t height = 0;
        uint32_t mipmap_levels = 0;
        vk::Format format = vk::Format::eR8G8B8A8Srgb;
        vk::ImageAspectFlags aspect_flags = vk::ImageAspectFlagBits::eColor;
        vk::ImageTiling tiling = vk::ImageTiling::eOptimal;
        vk::ImageUsageFlags usage_flags = vk::ImageUsageFlagBits::eTransferDst | vk::ImageUsageFlagBits::eSampled;
        bool compressed = false;
        bool allow_host_memory_mapping = false;

        constexpr bool validate() const
        {
            return (vkctx != nullptr) && (width != 0U) && (height != 0U) && (mipmap_levels >= 1);
        }
    };

    inline uint32_t get_max_mip_levels(uint32_t width, uint32_t height)
    {
        return static_cast<uint32_t>(std::floor(std::log2(std::max(width, height))) + 1);
    }

    class image
    {
    public:
        image(image_args);
        virtual ~image();

        void transition_layout_suboptimal(
            vk::ImageLayout old_layout,
            vk::ImageLayout new_layout,
            vk::CommandBuffer cmdbuff,
            vk::ImageAspectFlags aspect,
            uint32_t first_mip,
            uint32_t mip_count);

        vk::Image get_image() const { return _image; }

        uint32_t width() const { return _width; }

        uint32_t height() const { return _height; }

        vk::ImageAspectFlags aspect_flags() const { return _aspect_flags; }

        uint32_t mip_levels() const { return _mip_levels; }

        vk::Format format() const { return _format; }

        VmaAllocation allocation() const { return _allocation; }

        VmaAllocationInfo allocation_info() const { return _allocation_info; }

    private:
        const vulkan_context* _vkctx = nullptr;
        uint32_t _width = 0;
        uint32_t _height = 0;
        vk::ImageAspectFlags _aspect_flags;
        VkImage _image;
        vk::Format _format;
        VmaAllocation _allocation;
        VmaAllocationInfo _allocation_info;
        uint32_t _mip_levels = 0;
    };

    void transition_image_layout_suboptimal(
        vk::Image image,
        vk::ImageLayout old_layout,
        vk::ImageLayout new_layout,
        vk::ImageAspectFlags aspect,
        uint32_t first_mip,
        uint32_t mip_count,
        vk::CommandBuffer cmdbuff,
        const vulkan_context& vkctx);
} // namespace cathedral::gfx