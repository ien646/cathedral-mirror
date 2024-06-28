#include <cathedral/gfx/swapchain.hpp>

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
        vkb::SwapchainBuilder
            swapchain_builder(_vkctx.physdev(), _vkctx.device(), _vkctx.surface(), _vkctx.graphics_queue_family_index());
        swapchain_builder =
            swapchain_builder.use_default_format_selection()
                .set_desired_present_mode(static_cast<VkPresentModeKHR>(_present_mode))
                .set_desired_extent(surfsize.x, surfsize.y)
                .use_default_image_usage_flags();

        bool destroy_old_swapchain = false;
        if (_swapchain)
        {
            swapchain_builder = swapchain_builder.set_old_swapchain(_swapchain);
            destroy_old_swapchain = true;
        }

        auto swapchain_result = swapchain_builder.build();
        CRITICAL_CHECK(swapchain_result.has_value());

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
        CRITICAL_CHECK(images.has_value());
        std::copy(images.value().begin(), images.value().end(), std::back_inserter(_swapchain_images));
    }

    void swapchain::init_swapchain_imageviews()
    {
        _swapchain_imageviews.clear();

        auto imageviews = _swapchain.get_image_views();
        CRITICAL_CHECK(imageviews.has_value());
        std::copy(imageviews.value().begin(), imageviews.value().end(), std::back_inserter(_swapchain_imageviews));
    }

    void swapchain::init_cmdbuffs()
    {
        _transition_cmdbuff_undef2color.clear();
        _transition_cmdbuff_color2present.clear();
        
        for (size_t i = 0; i < _swapchain_images.size(); ++i)
        {
            vk::UniqueCommandBuffer cmdbuff = _vkctx.create_primary_commandbuffer();
            cmdbuff->begin(vk::CommandBufferBeginInfo{});
            {
                vk::ImageMemoryBarrier barrier;
                barrier.image = _swapchain_images[i];
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

                cmdbuff->pipelineBarrier(
                    vk::PipelineStageFlagBits::eAllCommands,
                    vk::PipelineStageFlagBits::eAllCommands,
                    (vk::DependencyFlags)0,
                    {},
                    {},
                    { barrier });
            }
            cmdbuff->end();
            _transition_cmdbuff_undef2color.push_back(std::move(cmdbuff));
        }

        for (size_t i = 0; i < _swapchain_images.size(); ++i)
        {
            vk::UniqueCommandBuffer cmdbuff = _vkctx.create_primary_commandbuffer();
            cmdbuff->begin(vk::CommandBufferBeginInfo{});
            {
                vk::ImageMemoryBarrier barrier;
                barrier.image = _swapchain_images[i];
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

                cmdbuff->pipelineBarrier(
                    vk::PipelineStageFlagBits::eAllCommands,
                    vk::PipelineStageFlagBits::eAllCommands,
                    (vk::DependencyFlags)0,
                    {},
                    {},
                    { barrier });
            }
            cmdbuff->end();
            _transition_cmdbuff_color2present.push_back(std::move(cmdbuff));
        }
    }

    void swapchain::recreate()
    {
        _vkctx.device().waitIdle();
        init_swapchain();
        init_swapchain_images();
        init_swapchain_imageviews();
        _image_ready_semaphore = _vkctx.create_default_semaphore();
        init_cmdbuffs();
    }

    uint32_t swapchain::acquire_next_image()
    {
        while (true)
        {
            auto acquire_result = _vkctx.device().acquireNextImageKHR(_swapchain.swapchain, UINT64_MAX, *_image_ready_semaphore);
            if (acquire_result.result == vk::Result::eErrorOutOfDateKHR ||
                acquire_result.result == vk::Result::eSuboptimalKHR)
            {
                recreate();
                _image_ready_semaphore = _vkctx.create_default_semaphore();
            }
            else if (acquire_result.result == vk::Result::eSuccess)
            {
                return acquire_result.value;
            }
        }
    }

    vk::Image swapchain::image(uint32_t index) const
    {
        CRITICAL_CHECK(index < _swapchain_images.size());
        return _swapchain_images[index];
    }

    vk::ImageView swapchain::imageview(uint32_t index) const
    {
        CRITICAL_CHECK(index < _swapchain_imageviews.size());
        return _swapchain_imageviews[index];
    }

    vk::CommandBuffer swapchain::transition_cmdbuff_undefined_color(uint32_t index) const
    {
        CRITICAL_CHECK(index < _transition_cmdbuff_undef2color.size());
        return *_transition_cmdbuff_undef2color[index];
    }

    vk::CommandBuffer swapchain::transition_cmdbuff_color_present(uint32_t index) const
    {
        CRITICAL_CHECK(index < _transition_cmdbuff_color2present.size());
        return *_transition_cmdbuff_color2present[index];
    }
} // namespace cathedral::gfx