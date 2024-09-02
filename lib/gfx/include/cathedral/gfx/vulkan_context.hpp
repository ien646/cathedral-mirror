#pragma once

#include <cathedral/gfx/vma_forward.hpp>

#include <glm/vec2.hpp>

#include <VkBootstrap.h>
#include <vulkan/vulkan.hpp>

#include <functional>

namespace cathedral::gfx
{
    class uniform_buffer;
    class storage_buffer;
    class image;
    class sampler;

    struct vulkan_context_args
    {
        std::function<vk::SurfaceKHR(vk::Instance)> surface_retriever = nullptr;
        std::function<glm::ivec2()> surface_size_retriever = nullptr;
        bool validation_layers = false;
        std::vector<const char*> instance_extensions;

        struct
        {
            uint32_t uniform_buffer_count = 10000;
            uint32_t storage_buffer_count = 10000;
            uint32_t combined_image_sampler_count = 10000;
            uint32_t max_sets = 10000;
        } descriptor_pool_args;
    };

    class vulkan_context
    {
    public:
        explicit vulkan_context(vulkan_context_args);
        ~vulkan_context() noexcept;

        vk::Instance instance() const;
        vk::PhysicalDevice physdev() const;
        vk::SurfaceKHR surface() const;
        vk::Device device() const;
        vk::Queue graphics_queue() const;
        uint32_t graphics_queue_family_index() const;
        const VmaAllocator& allocator() const;
        vk::CommandPool command_pool() const;
        vk::DescriptorPool descriptor_pool() const;
        vk::PipelineCache pipeline_cache() const;

        vk::Viewport get_default_viewport() const;
        vk::Rect2D get_default_scissor() const;

        vk::UniqueCommandBuffer create_primary_commandbuffer() const;
        void submit_commandbuffer_sync(vk::CommandBuffer cmdbuff) const;

        vk::UniqueSemaphore create_default_semaphore() const;
        vk::UniqueFence create_signaled_fence() const;

        glm::ivec2 get_surface_size() const;

    private:
        vkb::Instance _instance;
        vkb::PhysicalDevice _physdev;
        vk::SurfaceKHR _surface;
        vkb::Device _device;
        vk::Queue _graphics_queue;

        std::function<glm::ivec2()> _surface_size_retriever;

        VmaAllocator _allocator;

        vk::UniqueCommandPool _cmdpool;
        vk::UniqueDescriptorPool _descriptor_pool;

        vk::UniquePipelineCache _pipeline_cache;
    };
} // namespace cathedral::gfx
