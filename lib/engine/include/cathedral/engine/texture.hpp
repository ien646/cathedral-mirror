#pragma once

#include <cathedral/engine/texture_compression.hpp>

#include <cathedral/gfx/image.hpp>
#include <cathedral/gfx/sampler.hpp>

#include <ien/arithmetic.hpp>

namespace cathedral::engine
{
    class upload_queue;

    enum class texture_format
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

    constexpr bool is_compressed_format(texture_format fmt)
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

    constexpr texture_compression_type get_format_compression_type(texture_format fmt)
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

    constexpr bool is_texture_format_linear(texture_format fmt)
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

    constexpr uint32_t calc_texture_size(uint32_t width, uint32_t height, texture_format format)
    {
        switch (format)
        {
        case texture_format::R8G8B8A8_SRGB:
        case texture_format::R8G8B8A8_LINEAR:
            return width * height * 4;
        case texture_format::R8G8B8_SRGB:
        case texture_format::R8G8B8_LINEAR:
            return width * height * 3;
        case texture_format::R8G8_SRGB:
        case texture_format::R8G8_LINEAR:
            return width * height * 2;
        case texture_format::R8_SRGB:
        case texture_format::R8_LINEAR:
            return width * height;

        case texture_format::DXT1_BC1_SRGB:
        case texture_format::DXT1_BC1_LINEAR:
            CRITICAL_CHECK(ien::is_power_of_2(width));
            CRITICAL_CHECK(ien::is_power_of_2(height));
            return (width * height) / 2; // ((WxH) / 16) * 8

        case texture_format::DXT5_BC3_SRGB:
        case texture_format::DXT5_BC3_LINEAR:
            CRITICAL_CHECK(ien::is_power_of_2(width));
            CRITICAL_CHECK(ien::is_power_of_2(height));
            return width * height; // ((WxH) / 16) * 16
        default:
            CRITICAL_ERROR("Unhandled texture format");
        }
    }

    struct texture_args
    {
        const ien::image* pimage = nullptr;
        gfx::sampler_args sampler_args;
        uint32_t request_mipmap_levels = 1;
        vk::ImageAspectFlagBits image_aspect_flags = vk::ImageAspectFlagBits::eColor;
        ien::resize_filter mipgen_filter;
        texture_format format = texture_format::R8G8B8A8_SRGB;
        std::optional<std::string> path = std::nullopt;
    };

    class texture
    {
    public:
        texture(texture_args args, upload_queue& queue);

        const gfx::sampler& sampler() const { return *_sampler; }
        const gfx::image& image() const { return *_image; }
        const vk::ImageView imageview() const { return *_imageview; }
        const std::optional<std::string>& path() const { return _path; }

    private:
        std::unique_ptr<gfx::image> _image;
        vk::UniqueImageView _imageview;
        std::unique_ptr<gfx::sampler> _sampler;
        std::optional<std::string> _path;
    };
} // namespace cathedral::engine