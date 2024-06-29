#pragma once

#include <cathedral/core.hpp>
#include <cathedral/gfx/vulkan_context.hpp>

#include <VkBootstrap.h>

namespace cathedral::gfx
{
    class swapchain
    {
    public:
        explicit swapchain(vulkan_context& vkctx, vk::PresentModeKHR initial_present_mode);

        void recreate();

        inline vk::Semaphore image_ready_semaphore() const { return *_image_ready_semaphore; }

        uint32_t acquire_next_image();

        vk::Image image(uint32_t index) const;
        vk::ImageView imageview(uint32_t index) const;

        inline vk::SwapchainKHR get() const { return _swapchain.swapchain; }

        vk::CommandBuffer transition_cmdbuff_undefined_color(uint32_t index) const;
        vk::CommandBuffer transition_cmdbuff_color_present(uint32_t index) const;

    private:
        vulkan_context& _vkctx;
        vk::PresentModeKHR _present_mode;
        vkb::Swapchain _swapchain;
        std::vector<vk::Image> _swapchain_images;
        std::vector<vk::ImageView> _swapchain_imageviews;
        std::vector<vk::UniqueCommandBuffer> _transition_cmdbuff_undef2color;
        std::vector<vk::UniqueCommandBuffer> _transition_cmdbuff_color2present;
        vk::UniqueSemaphore _image_ready_semaphore;

        void init_swapchain();
        void init_swapchain_images();
        void init_swapchain_imageviews();
        void init_cmdbuffs();
    };
} // namespace cathedral::gfx