#pragma once

#include <cathedral/gfx/vulkan_context.hpp>

namespace cathedral::gfx
{
    struct sampler_args
    {
        const vulkan_context* vkctx = nullptr;
        vk::SamplerAddressMode address_mode = vk::SamplerAddressMode::eRepeat;
        vk::SamplerMipmapMode mipmap_mode = vk::SamplerMipmapMode::eLinear;
        vk::Filter mag_filter = vk::Filter::eLinear;
        vk::Filter min_filter = vk::Filter::eLinear;
        uint32_t anisotropy_level = 0;
    };

    class sampler
    {
    public:
        explicit sampler(sampler_args);

        inline vk::Sampler get_sampler() const { return *_sampler; }

    private:
        vk::UniqueSampler _sampler;
        sampler_args _args;
    };
} // namespace cathedral::gfx