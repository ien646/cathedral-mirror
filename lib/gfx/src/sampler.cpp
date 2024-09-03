#include <cathedral/gfx/sampler.hpp>

namespace cathedral::gfx
{
    sampler::sampler(sampler_args args)
        : _args(std::move(args))
    {
        vk::SamplerCreateInfo info;
        info.addressModeU = args.address_mode;
        info.addressModeV = args.address_mode;
        info.addressModeW = args.address_mode;
        info.mipmapMode = args.mipmap_mode;
        info.anisotropyEnable = args.anisotropy_level > 0;
        info.maxAnisotropy = static_cast<float>(args.anisotropy_level);
        info.borderColor = vk::BorderColor::eFloatOpaqueBlack;
        info.compareEnable = false;
        info.magFilter = args.mag_filter;
        info.minFilter = args.min_filter;
        info.maxLod = VK_LOD_CLAMP_NONE;
        info.minLod = 0;
        info.mipLodBias = 0.0f;
        info.unnormalizedCoordinates = false;

        _sampler = args.vkctx->device().createSamplerUnique(info);
    }
} // namespace cathedral::gfx