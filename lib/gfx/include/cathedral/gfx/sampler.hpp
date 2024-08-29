#pragma once

#include <cathedral/gfx/vulkan_context.hpp>

namespace cathedral::gfx
{
    struct sampler_args
    {
        const vulkan_context* vkctx = nullptr;
        vk::SamplerAddressMode address_mode = vk::SamplerAddressMode::eRepeat;
        vk::SamplerMipmapMode mipmap_mode = vk::SamplerMipmapMode::eLinear;
        vk::Filter mag_filter;
        vk::Filter min_filter;
        uint32_t anisotropy_level = 0;
    };

    class sampler
    {
    public:
        sampler(sampler_args);

        inline vk::Sampler get_sampler() const { return *_sampler; }

    private:
        vk::UniqueSampler _sampler;
    };
}