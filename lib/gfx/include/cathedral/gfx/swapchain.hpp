#pragma once

#include <cathedral/core.hpp>

#include <vulkan/vulkan.hpp>

#include <VkBootstrap.h>

#include <functional>

namespace cathedral::gfx
{
    FORWARD_CLASS_INLINE(vulkan_context);

    class swapchain
    {
    public:
        explicit swapchain(vulkan_context& vkctx, vk::PresentModeKHR initial_present_mode);

        void recreate();

        vk::Semaphore image_ready_semaphore() const { return *_image_ready_semaphore; }

        uint32_t acquire_next_image(const std::function<void()>& swapchain_recreate_callback);

        vk::Image image(uint32_t index) const;
        vk::ImageView imageview(uint32_t index) const;

        size_t image_count() const { return _swapchain_images.size(); }

        vk::SwapchainKHR get() const { return _swapchain.swapchain; }

        void transition_undefined_color(uint32_t index, vk::CommandBuffer cmdbuff) const;
        void transition_color_present(uint32_t index, vk::CommandBuffer cmdbuff) const;

        vk::Format swapchain_image_format() const { return static_cast<vk::Format>(_swapchain.image_format); }

        vulkan_context& vkctx() const { return _vkctx; }

        void set_present_mode(const vk::PresentModeKHR mode) { _present_mode = mode; }

        VkExtent2D extent() const { return _swapchain.extent; }

    private:
        vulkan_context& _vkctx;
        vk::PresentModeKHR _present_mode;
        vkb::Swapchain _swapchain;
        std::vector<vk::Image> _swapchain_images;
        std::vector<vk::ImageView> _swapchain_imageviews;
        vk::UniqueSemaphore _image_ready_semaphore;

        void init_swapchain();
        void init_swapchain_images();
        void init_swapchain_imageviews();
    };
} // namespace cathedral::gfx