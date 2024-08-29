#pragma once

#include <cathedral/core.hpp>
#include <cathedral/gfx/vulkan_context.hpp>

#include <ien/image/image.hpp>

#include <vk_mem_alloc.h>

namespace cathedral::gfx
{
    struct image_args
    {
        const vulkan_context* vkctx = nullptr;
        uint32_t width = 0;
        uint32_t height = 0;
        uint32_t mipmap_levels = 0;
        vk::Format format = vk::Format::eR8G8B8A8Srgb;
        vk::ImageAspectFlags aspect_flags = vk::ImageAspectFlagBits::eColor;
        bool compressed = false;

        inline constexpr bool validate() const { return vkctx && width && height && (mipmap_levels >= 1); }
    };

    class image
    {
    public:
        image(image_args);
        virtual ~image();

        void transition_layout(
            vk::ImageLayout old_layout,
            vk::ImageLayout new_layout,
            vk::CommandBuffer cmdbuff,
            vk::ImageAspectFlags aspect,
            uint32_t first_mip,
            uint32_t mip_count);

        inline vk::Image get_image() const { return _image; }
        inline uint32_t width() const { return _width; }
        inline uint32_t height() const { return _height; }
        inline vk::ImageAspectFlags aspect_flags() const { return _aspect_flags; }
        inline uint32_t mip_levels() const { return _mip_levels; }
        inline vk::Format format() const { return _format; }

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
} // namespace cathedral::gfx