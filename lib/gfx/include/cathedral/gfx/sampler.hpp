#pragma once

#include <cathedral/core.hpp>

#include <vulkan/vulkan.hpp>

namespace cathedral::gfx
{
    FORWARD_CLASS_INLINE(vulkan_context);

    struct sampler_info
    {
        vk::SamplerAddressMode address_mode = vk::SamplerAddressMode::eRepeat;
        vk::SamplerMipmapMode mipmap_mode = vk::SamplerMipmapMode::eLinear;
        vk::Filter mag_filter = vk::Filter::eLinear;
        vk::Filter min_filter = vk::Filter::eLinear;
        uint32_t anisotropy_level = 0;
    };

    class sampler
    {
    public:
        sampler(const vulkan_context* vkctx, sampler_info info);

        vk::Sampler get_sampler() const { return *_sampler; }

    private:
        vk::UniqueSampler _sampler;
        const vulkan_context* _vkctx;
        sampler_info _info;
    };
} // namespace cathedral::gfx