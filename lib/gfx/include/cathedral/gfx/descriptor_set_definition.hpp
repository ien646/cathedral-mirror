#pragma once

#include <cathedral/core.hpp>

#include <cathedral/gfx/types.hpp>

#include <vulkan/vulkan.hpp>

namespace cathedral::gfx
{
    FORWARD_CLASS_INLINE(vulkan_context);

    struct descriptor_set_entry
    {
        uint32_t set;
        uint32_t binding;
        descriptor_type type;
        uint32_t count;

        constexpr descriptor_set_entry(
            const uint32_t set,
            const uint32_t binding,
            const descriptor_type type,
            const uint32_t count)
            : set(set)
            , binding(binding)
            , type(type)
            , count(count)
        {
        }
    };

    struct descriptor_set_definition
    {
        std::vector<descriptor_set_entry> entries;

        bool validate() const;

        vk::UniqueDescriptorSetLayout create_descriptor_set_layout(const vulkan_context& vkctx) const;
    };

    vk::DescriptorType to_vk_descriptor_type(descriptor_type);
} // namespace cathedral::gfx