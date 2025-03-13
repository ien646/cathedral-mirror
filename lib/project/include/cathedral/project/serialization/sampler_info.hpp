#pragma once

#include <cathedral/gfx/sampler.hpp>

#include <cereal/cereal.hpp>

namespace cereal
{
    template <typename Archive>
    void CEREAL_SERIALIZE_FUNCTION_NAME(Archive& ar, cathedral::gfx::sampler_info& args)
    {
        ar(cereal::make_nvp("address_mode", args.address_mode),
           cereal::make_nvp("mipmap_mode", args.mipmap_mode),
           cereal::make_nvp("anisotropy_level", args.anisotropy_level),
           cereal::make_nvp("mag_filter", args.mag_filter),
           cereal::make_nvp("min_filter", args.min_filter));
    }
} // namespace cereal