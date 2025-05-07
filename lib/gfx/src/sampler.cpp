#include <cathedral/gfx/sampler.hpp>

#include <cathedral/gfx/vulkan_context.hpp>

namespace cathedral::gfx
{
    sampler::sampler(const vulkan_context* vkctx, sampler_info info)
        : _vkctx(vkctx)
        , _info(std::move(info))
    {
        vk::SamplerCreateInfo create_info;
        create_info.addressModeU = _info.address_mode;
        create_info.addressModeV = _info.address_mode;
        create_info.addressModeW = _info.address_mode;
        create_info.mipmapMode = _info.mipmap_mode;
        create_info.anisotropyEnable = static_cast<vk::Bool32>(_info.anisotropy_level > 0);
        create_info.maxAnisotropy = static_cast<float>(_info.anisotropy_level);
        create_info.borderColor = vk::BorderColor::eFloatOpaqueBlack;
        create_info.compareEnable = vk::False;
        create_info.magFilter = _info.mag_filter;
        create_info.minFilter = _info.min_filter;
        create_info.maxLod = VK_LOD_CLAMP_NONE;
        create_info.minLod = 0;
        create_info.mipLodBias = 0.0F;
        create_info.unnormalizedCoordinates = vk::False;

        _sampler = _vkctx->device().createSamplerUnique(create_info);
    }
} // namespace cathedral::gfx