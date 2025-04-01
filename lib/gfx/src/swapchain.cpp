#include <cathedral/gfx/swapchain.hpp>

#include <cathedral/gfx/vulkan_context.hpp>

namespace cathedral::gfx
{
    swapchain::swapchain(vulkan_context& vkctx, vk::PresentModeKHR initial_present_mode)
        : _vkctx(vkctx)
        , _present_mode(initial_present_mode)
    {
        recreate();
    }

    void swapchain::init_swapchain()
    {
        const auto surfsize = _vkctx.get_surface_size();
        vkb::SwapchainBuilder swapchain_builder(
            _vkctx.physdev(),
            _vkctx.device(),
            _vkctx.surface(),
            _vkctx.graphics_queue_family_index());
        swapchain_builder =
            swapchain_builder.use_default_format_selection()
                .set_desired_present_mode(static_cast<VkPresentModeKHR>(_present_mode))
                .set_desired_extent(surfsize.x, surfsize.y)
                // VK_IMAGE_USAGE_TRANSFER_SRC_BIT is required to blit images to capture screenshots
                .set_image_usage_flags(VK_IMAGE_USAGE_TRANSFER_SRC_BIT | VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT);

        bool destroy_old_swapchain = false;
        if (_swapchain != nullptr)
        {
            swapchain_builder = swapchain_builder.set_old_swapchain(_swapchain);
            destroy_old_swapchain = true;
        }

        auto swapchain_result = swapchain_builder.build();
        CRITICAL_CHECK(swapchain_result.has_value(), "Swapchain creation failure");

        if (destroy_old_swapchain)
        {
            vkb::destroy_swapchain(_swapchain);
        }
        _swapchain = swapchain_result.value();
    }

    void swapchain::init_swapchain_images()
    {
        _swapchain_images.clear();

        auto images = _swapchain.get_images();
        CRITICAL_CHECK(images.has_value(), "Invalid swapchain images");
        std::ranges::copy(images.value(), std::back_inserter(_swapchain_images));
    }

    void swapchain::init_swapchain_imageviews()
    {
        _swapchain_imageviews.clear();

        auto imageviews = _swapchain.get_image_views();
        CRITICAL_CHECK(imageviews.has_value(), "Invalid swapchain imageviews");
        std::ranges::copy(imageviews.value(), std::back_inserter(_swapchain_imageviews));
    }

    void swapchain::recreate()
    {
        _vkctx.device().waitIdle();
        init_swapchain();
        init_swapchain_images();
        init_swapchain_imageviews();
        _image_ready_semaphore = _vkctx.create_default_semaphore();
    }

    uint32_t swapchain::acquire_next_image(const std::function<void()>& swapchain_recreate_callback)
    {
        while (true)
        {
            vk::ResultValue<uint32_t> acquire_result = { vk::Result::eErrorUnknown, 0 };
            try
            {
                acquire_result =
                    _vkctx.device().acquireNextImageKHR(_swapchain.swapchain, 1000000000, *_image_ready_semaphore);
            }
            catch (const std::exception& err)
            {
                if (dynamic_cast<const vk::OutOfDateKHRError*>(&err) != nullptr)
                {
                    recreate();
                    _image_ready_semaphore = _vkctx.create_default_semaphore();
                    swapchain_recreate_callback();
                    continue;
                }
            }

            if (acquire_result.result == vk::Result::eErrorOutOfDateKHR ||
                acquire_result.result == vk::Result::eSuboptimalKHR)
            {
                recreate();
                _image_ready_semaphore = _vkctx.create_default_semaphore();
                swapchain_recreate_callback();
            }
            else if (acquire_result.result == vk::Result::eSuccess)
            {
                return acquire_result.value;
            }
            else
            {
                CRITICAL_ERROR("Unhandled result from acquireNextImageKHR");
            }
        }
    }

    vk::Image swapchain::image(uint32_t index) const
    {
        CRITICAL_CHECK(index < _swapchain_images.size(), "Invalid swapchain image index");
        return _swapchain_images[index];
    }

    vk::ImageView swapchain::imageview(uint32_t index) const
    {
        CRITICAL_CHECK(index < _swapchain_imageviews.size(), "Invalid swapchain imageview index");
        return _swapchain_imageviews[index];
    }

    void swapchain::transition_undefined_color(uint32_t index, vk::CommandBuffer cmdbuff) const
    {
        vk::ImageMemoryBarrier barrier;
        barrier.image = _swapchain_images[index];
        barrier.oldLayout = vk::ImageLayout::eUndefined;
        barrier.newLayout = vk::ImageLayout::eColorAttachmentOptimal;
        barrier.srcAccessMask = vk::AccessFlagBits::eMemoryRead | vk::AccessFlagBits::eMemoryWrite;
        barrier.srcQueueFamilyIndex = _vkctx.graphics_queue_family_index();
        barrier.dstAccessMask = vk::AccessFlagBits::eMemoryRead | vk::AccessFlagBits::eMemoryWrite;
        barrier.dstQueueFamilyIndex = _vkctx.graphics_queue_family_index();
        barrier.subresourceRange.aspectMask = vk::ImageAspectFlagBits::eColor;
        barrier.subresourceRange.baseArrayLayer = 0;
        barrier.subresourceRange.baseMipLevel = 0;
        barrier.subresourceRange.layerCount = 1;
        barrier.subresourceRange.levelCount = 1;

        cmdbuff.pipelineBarrier(
            vk::PipelineStageFlagBits::eAllCommands,
            vk::PipelineStageFlagBits::eAllCommands,
            (vk::DependencyFlags)0,
            {},
            {},
            { barrier });
    }

    void swapchain::transition_color_present(uint32_t index, vk::CommandBuffer cmdbuff) const
    {
        vk::ImageMemoryBarrier barrier;
        barrier.image = _swapchain_images[index];
        barrier.oldLayout = vk::ImageLayout::eColorAttachmentOptimal;
        barrier.newLayout = vk::ImageLayout::ePresentSrcKHR;
        barrier.srcAccessMask = vk::AccessFlagBits::eMemoryRead | vk::AccessFlagBits::eMemoryWrite;
        barrier.srcQueueFamilyIndex = _vkctx.graphics_queue_family_index();
        barrier.dstAccessMask = vk::AccessFlagBits::eMemoryRead | vk::AccessFlagBits::eMemoryWrite;
        barrier.dstQueueFamilyIndex = _vkctx.graphics_queue_family_index();
        barrier.subresourceRange.aspectMask = vk::ImageAspectFlagBits::eColor;
        barrier.subresourceRange.baseArrayLayer = 0;
        barrier.subresourceRange.baseMipLevel = 0;
        barrier.subresourceRange.layerCount = 1;
        barrier.subresourceRange.levelCount = 1;

        cmdbuff.pipelineBarrier(
            vk::PipelineStageFlagBits::eAllCommands,
            vk::PipelineStageFlagBits::eAllCommands,
            (vk::DependencyFlags)0,
            {},
            {},
            { barrier });
    }
} // namespace cathedral::gfx