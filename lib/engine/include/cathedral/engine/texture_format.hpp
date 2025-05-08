#pragma once

#include <cathedral/core.hpp>

#include <cathedral/engine/texture_compression.hpp>

namespace cathedral::engine
{
    enum class texture_format : uint8_t
    {
        R8G8B8A8_SRGB,
        R8G8B8_SRGB,
        R8G8_SRGB,
        R8_SRGB,
        R8G8B8A8_LINEAR,
        R8G8B8_LINEAR,
        R8G8_LINEAR,
        R8_LINEAR,

        DXT1_BC1_SRGB,
        DXT5_BC3_SRGB,

        DXT1_BC1_LINEAR,
        DXT5_BC3_LINEAR
    };

    constexpr bool is_compressed_format(const texture_format fmt)
    {
        switch (fmt)
        {
        case texture_format::DXT1_BC1_LINEAR:
        case texture_format::DXT5_BC3_LINEAR:
        case texture_format::DXT1_BC1_SRGB:
        case texture_format::DXT5_BC3_SRGB:
            return true;
        default:
            return false;
        }
    }

    constexpr texture_compression_type get_format_compression_type(const texture_format fmt)
    {
        switch (fmt)
        {
        case texture_format::DXT1_BC1_LINEAR:
        case texture_format::DXT1_BC1_SRGB:
            return texture_compression_type::DXT1_BC1;
        case texture_format::DXT5_BC3_SRGB:
        case texture_format::DXT5_BC3_LINEAR:
            return texture_compression_type::DXT5_BC3;
        default:
            CRITICAL_ERROR("Unhandled compressed format");
        }
    }

    constexpr bool is_texture_format_linear(const texture_format fmt)
    {
        using enum texture_format;
        switch (fmt)
        {
        case R8G8B8A8_SRGB:
        case R8G8B8_SRGB:
        case R8G8_SRGB:
        case R8_SRGB:
        case DXT1_BC1_SRGB:
        case DXT5_BC3_SRGB:
            return false;

        case R8G8B8A8_LINEAR:
        case R8G8B8_LINEAR:
        case R8G8_LINEAR:
        case R8_LINEAR:
        case DXT1_BC1_LINEAR:
        case DXT5_BC3_LINEAR:
            return true;
        }
        CRITICAL_ERROR("Unhandled texture format");
    }
} // namespace cathedral::engine