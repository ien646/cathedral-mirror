#include <cathedral/gfx/vulkan_context.hpp>

#include <cathedral/core.hpp>

#include <vk_mem_alloc.h>

VULKAN_HPP_DEFAULT_DISPATCH_LOADER_DYNAMIC_STORAGE;

namespace cathedral::gfx
{
    vulkan_context::vulkan_context(vulkan_context_args args)
        : _surface_size_retriever(args.surface_size_retriever)
    {
        CRITICAL_CHECK(args.surface_retriever != nullptr);
        CRITICAL_CHECK(args.surface_size_retriever != nullptr);

        // Dispatcher init
        VULKAN_HPP_DEFAULT_DISPATCHER.init();

        // Init instance
        vkb::InstanceBuilder instance_builder;
        auto inst = instance_builder.enable_validation_layers(args.validation_layers)
                        .require_api_version(1, 3, 0)
                        .set_minimum_instance_version(1, 3, 0)
                        .use_default_debug_messenger()
                        .enable_extensions(args.instance_extensions)
                        .build();

        if (!inst)
        {
            CRITICAL_ERROR("Failure creating vulkan instance: " + inst.error().message());
        }

        CRITICAL_CHECK(inst.has_value());
        _instance = inst.value();

        VULKAN_HPP_DEFAULT_DISPATCHER.init(vk::Instance(_instance.instance));

        // Init surface
        _surface = args.surface_retriever(_instance.instance);
        CRITICAL_CHECK(_surface);

        // Init physical device
        auto features = zero_struct<VkPhysicalDeviceFeatures>();
        features.samplerAnisotropy = vk::True;

        auto features_12 = zero_struct<VkPhysicalDeviceVulkan12Features>();
        //features_12.bufferDeviceAddress = vk::True;

        auto features_13 = zero_struct<VkPhysicalDeviceVulkan13Features>();
        features_13.dynamicRendering = vk::True;
        features_13.synchronization2 = vk::True;

        vkb::PhysicalDeviceSelector pdev_selector(_instance);
        auto pdev = pdev_selector.prefer_gpu_device_type(vkb::PreferredDeviceType::discrete)
                        .require_present(true)
                        .set_surface(_surface)
                        .set_required_features(features)
                        .set_required_features_12(features_12)
                        .set_required_features_13(features_13)
                        .select();

        CRITICAL_CHECK(pdev.has_value());
        _physdev = pdev.value();

        // Init device
        vkb::DeviceBuilder dev_builder(_physdev);
        auto dev = dev_builder.build();

        CRITICAL_CHECK(dev.has_value());
        _device = dev.value();

        VULKAN_HPP_DEFAULT_DISPATCHER.init(vk::Device(_device.device));

        // Init queue
        auto gfx_queue = _device.get_queue(vkb::QueueType::graphics);
        CRITICAL_CHECK(gfx_queue.has_value());
        _graphics_queue = gfx_queue.value();

        // Init allocator
        auto allocator_info = zero_struct<VmaAllocatorCreateInfo>();
        allocator_info.device = device();
        allocator_info.instance = instance();
        allocator_info.physicalDevice = physdev();
        allocator_info.vulkanApiVersion = VK_API_VERSION_1_3;

        auto allocator_create_result = vmaCreateAllocator(&allocator_info, &_allocator);
        CRITICAL_CHECK(allocator_create_result == VkResult::VK_SUCCESS);

        // Init commandpool
        vk::CommandPoolCreateInfo cmdpool_info;
        cmdpool_info.flags = vk::CommandPoolCreateFlagBits::eResetCommandBuffer;
        cmdpool_info.queueFamilyIndex = graphics_queue_family_index();
        _cmdpool = device().createCommandPoolUnique(cmdpool_info);

        // Init descriptor pool
        std::vector<vk::DescriptorPoolSize> dpool_sizes = {
            { .type = vk::DescriptorType::eUniformBuffer, .descriptorCount = args.descriptor_pool_args.uniform_buffer_count },
            { .type = vk::DescriptorType::eStorageBuffer, .descriptorCount = args.descriptor_pool_args.storage_buffer_count },
            { .type = vk::DescriptorType::eCombinedImageSampler,
              .descriptorCount = args.descriptor_pool_args.combined_image_sampler_count }
        };
        vk::DescriptorPoolCreateInfo dpool_info;
        dpool_info.poolSizeCount = static_cast<uint32_t>(dpool_sizes.size());
        dpool_info.pPoolSizes = dpool_sizes.data();
        dpool_info.maxSets = args.descriptor_pool_args.max_sets;
        _descriptor_pool = device().createDescriptorPoolUnique(dpool_info);

        // Pipeline cache
        vk::PipelineCacheCreateInfo pipeline_cache_info;
        pipeline_cache_info.initialDataSize = 0;
        pipeline_cache_info.pInitialData = nullptr;
        _pipeline_cache = device().createPipelineCacheUnique(pipeline_cache_info);
    }

    vulkan_context::~vulkan_context() noexcept
    {
        try
        {
            device().waitIdle();
        }
        catch (const std::exception&)
        {
            exit(-1);
        }
    }

    vk::Instance vulkan_context::instance() const
    {
        return _instance.instance;
    }

    vk::PhysicalDevice vulkan_context::physdev() const
    {
        return _physdev.physical_device;
    }

    vk::SurfaceKHR vulkan_context::surface() const
    {
        return _surface;
    }

    vk::Device vulkan_context::device() const
    {
        return _device.device;
    }

    vk::Queue vulkan_context::graphics_queue() const
    {
        return _graphics_queue;
    }

    uint32_t vulkan_context::graphics_queue_family_index() const
    {
        return _device.get_queue_index(vkb::QueueType::graphics).value();
    }

    const VmaAllocator& vulkan_context::allocator() const
    {
        return _allocator;
    }

    vk::CommandPool vulkan_context::command_pool() const
    {
        return *_cmdpool;
    }

    vk::DescriptorPool vulkan_context::descriptor_pool() const
    {
        return *_descriptor_pool;
    }

    vk::PipelineCache vulkan_context::pipeline_cache() const
    {
        return *_pipeline_cache;
    }

    vk::Viewport vulkan_context::get_default_viewport() const
    {
        const auto wsz = get_surface_size();
        vk::Viewport viewport;
        viewport.x = 0;
        viewport.y = 0;
        viewport.width = static_cast<float>(wsz.x);
        viewport.height = static_cast<float>(wsz.y);
        viewport.minDepth = 0.0F;
        viewport.maxDepth = 1.0F;
        return viewport;
    }

    vk::Rect2D vulkan_context::get_default_scissor() const
    {
        const auto wsz = get_surface_size();
        vk::Rect2D scissor;
        scissor.offset = vk::Offset2D{ .x = 0, .y = 0 };
        scissor.extent = vk::Extent2D{ .width = static_cast<uint32_t>(wsz.x), .height = static_cast<uint32_t>(wsz.y) };
        return scissor;
    }

    vk::UniqueCommandBuffer vulkan_context::create_primary_commandbuffer() const
    {
        vk::CommandBufferAllocateInfo info;
        info.commandBufferCount = 1;
        info.commandPool = *_cmdpool;
        info.level = vk::CommandBufferLevel::ePrimary;
        auto result = device().allocateCommandBuffersUnique(info);
        return std::move(result[0]);
    }

    void vulkan_context::submit_commandbuffer_sync(vk::CommandBuffer cmdbuff) const
    {
        vk::SubmitInfo submit;
        submit.signalSemaphoreCount = 0;
        submit.waitSemaphoreCount = 0;
        submit.commandBufferCount = 1;
        submit.pCommandBuffers = &cmdbuff;
        graphics_queue().submit(submit);
        graphics_queue().waitIdle();
    }

    vk::UniqueSemaphore vulkan_context::create_default_semaphore() const
    {
        vk::SemaphoreCreateInfo info;
        return device().createSemaphoreUnique(info);
    }

    vk::UniqueFence vulkan_context::create_signaled_fence() const
    {
        vk::FenceCreateInfo info;
        info.flags = vk::FenceCreateFlagBits::eSignaled;
        return device().createFenceUnique(info);
    }

    glm::ivec2 vulkan_context::get_surface_size() const
    {
        return _surface_size_retriever();
    }
} // namespace cathedral::gfx
