#include <cathedral/gfx/sampler.hpp>

namespace cathedral::gfx
{
    sampler::sampler(sampler_args args)
        : _args(std::move(args))
    {
        vk::SamplerCreateInfo info;
        info.addressModeU = _args.address_mode;
        info.addressModeV = _args.address_mode;
        info.addressModeW = _args.address_mode;
        info.mipmapMode = _args.mipmap_mode;
        info.anisotropyEnable = vk::Bool32(_args.anisotropy_level > 0);
        info.maxAnisotropy = static_cast<float>(_args.anisotropy_level);
        info.borderColor = vk::BorderColor::eFloatOpaqueBlack;
        info.compareEnable = vk::False;
        info.magFilter = _args.mag_filter;
        info.minFilter = _args.min_filter;
        info.maxLod = VK_LOD_CLAMP_NONE;
        info.minLod = 0;
        info.mipLodBias = 0.0F;
        info.unnormalizedCoordinates = vk::False;

        _sampler = _args.vkctx->device().createSamplerUnique(info);
    }
} // namespace cathedral::gfx