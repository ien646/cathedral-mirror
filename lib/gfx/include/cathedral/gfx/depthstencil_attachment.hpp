#pragma once

#include <cathedral/gfx/vma_forward.hpp>
#include <cathedral/gfx/vulkan_context.hpp>

namespace cathedral::gfx
{
    struct depthstencil_attachment_args
    {
        const vulkan_context* vkctx = nullptr;
        size_t width = 0;
        size_t height = 0;
    };

    class depthstencil_attachment
    {
    public:
        depthstencil_attachment(depthstencil_attachment_args);
        depthstencil_attachment(const depthstencil_attachment&) = delete;
        depthstencil_attachment(depthstencil_attachment&&) = default;
        ~depthstencil_attachment();

        depthstencil_attachment& operator=(const depthstencil_attachment&) = delete;
        depthstencil_attachment& operator=(depthstencil_attachment&&) = default;

        inline vk::Image image() const { return _image; }

        inline vk::ImageView depth_imageview() const { return *_depth_imageview; }

        inline vk::ImageView stencil_imageview() const { return *_stencil_imageview; }

        inline vk::ImageView depthstencil_imageview() const { return *_depthstencil_imageview; }

        inline size_t width() const { return _args.width; }

        inline size_t height() const { return _args.height; }

        inline constexpr static vk::Format format() { return vk::Format::eD32SfloatS8Uint; }

        void reload(depthstencil_attachment_args);

    private:
        depthstencil_attachment_args _args;
        VkImage _image = VK_NULL_HANDLE;
        vk::UniqueImageView _depth_imageview = {};
        vk::UniqueImageView _stencil_imageview = {};
        vk::UniqueImageView _depthstencil_imageview = {};
        VmaAllocation* _image_allocation = nullptr;
        VmaAllocationInfo* _image_allocation_info = nullptr;

        void unload();
        void load();
    };
} // namespace cathedral::gfx