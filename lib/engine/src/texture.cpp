#include <cathedral/engine/texture.hpp>

#include <cathedral/engine/texture_compression.hpp>
#include <cathedral/engine/texture_mip.hpp>
#include <cathedral/engine/upload_queue.hpp>

#include <ien/initializers.hpp>

#include <cmath>

namespace cathedral::engine
{
    vk::Format tex_fmt_to_vk_fmt(texture_format fmt)
    {
        switch (fmt)
        {
        case texture_format::DXT1_BC1_SRGB:
            return vk::Format::eBc1RgbSrgbBlock;
        case texture_format::DXT5_BC3_SRGB:
            return vk::Format::eBc3SrgbBlock;

        case texture_format::DXT1_BC1_LINEAR:
            return vk::Format::eBc1RgbUnormBlock;
        case texture_format::DXT5_BC3_LINEAR:
            return vk::Format::eBc3UnormBlock;

        case texture_format::R8_SRGB:
            return vk::Format::eR8Srgb;
        case texture_format::R8G8_SRGB:
            return vk::Format::eR8G8Srgb;
        case texture_format::R8G8B8_SRGB:
            return vk::Format::eR8G8B8Srgb;
        case texture_format::R8G8B8A8_SRGB:
            return vk::Format::eR8G8B8A8Srgb;

        case texture_format::R8_LINEAR:
            return vk::Format::eR8G8B8A8Unorm;
        case texture_format::R8G8_LINEAR:
            return vk::Format::eR8G8Unorm;
        case texture_format::R8G8B8_LINEAR:
            return vk::Format::eR8G8B8Unorm;
        case texture_format::R8G8B8A8_LINEAR:
            return vk::Format::eR8G8B8A8Unorm;
        }
        CRITICAL_ERROR("Unhandled ien::image_format");
    }

    constexpr ien::image_format uncompressed_type_to_ien_format(texture_format fmt)
    {
        switch (fmt)
        {
        case texture_format::R8_SRGB:
        case texture_format::R8_LINEAR:
            return ien::image_format::R;
        case texture_format::R8G8_SRGB:
        case texture_format::R8G8_LINEAR:
            return ien::image_format::RG;
        case texture_format::R8G8B8_SRGB:
        case texture_format::R8G8B8_LINEAR:
            return ien::image_format::RGB;
        case texture_format::R8G8B8A8_SRGB:
        case texture_format::R8G8B8A8_LINEAR:
            return ien::image_format::RGBA;
        default:
            CRITICAL_ERROR("Unhandled uncompressed format");
        }
    }

    constexpr vk::Format get_imageview_format(vk::Format format)
    {
        using enum vk::Format;
        switch (format)
        {
        case eBc1RgbSrgbBlock:
            return eR8G8B8Srgb;
        case eBc1RgbaSrgbBlock:
        case eBc3SrgbBlock:
            return eR8G8B8A8Srgb;

        case eBc1RgbUnormBlock:
            return eR8G8B8Unorm;
        case eBc1RgbaUnormBlock:
        case eBc3UnormBlock:
            return eR8G8B8A8Unorm;

        default:
            return format;
        }
    }

    texture::texture(texture_args args, upload_queue& queue)
        : _path(std::move(args.path))
    {
        CRITICAL_CHECK(args.request_mipmap_levels > 0);

        gfx::image_args image_args;
        image_args.vkctx = &queue.vkctx();
        image_args.aspect_flags = args.image_aspect_flags;
        image_args.width = static_cast<uint32_t>(args.pimage->width());
        image_args.height = static_cast<uint32_t>(args.pimage->height());
        image_args.format = tex_fmt_to_vk_fmt(args.format);
        image_args.mipmap_levels = args.request_mipmap_levels;
        image_args.compressed = is_compressed_format(args.format);

        _image = std::make_unique<gfx::image>(image_args);
        _sampler = std::make_unique<gfx::sampler>(args.sampler_args);

        vk::ImageViewCreateInfo imageview_info;
        imageview_info.image = _image->get_image();
        imageview_info.viewType = vk::ImageViewType::e2D;
        imageview_info.components.r = vk::ComponentSwizzle::eIdentity;
        imageview_info.components.g = vk::ComponentSwizzle::eIdentity;
        imageview_info.components.b = vk::ComponentSwizzle::eIdentity;
        imageview_info.components.a = vk::ComponentSwizzle::eIdentity;
        imageview_info.format = tex_fmt_to_vk_fmt(args.format);
        imageview_info.subresourceRange.aspectMask = args.image_aspect_flags;
        imageview_info.subresourceRange.baseArrayLayer = 0;
        imageview_info.subresourceRange.baseMipLevel = 0;
        imageview_info.subresourceRange.layerCount = 1;
        imageview_info.subresourceRange.levelCount = _image->mip_levels();

        _imageview = queue.vkctx().device().createImageViewUnique(imageview_info);

        // Transition all mips to transferDst
        queue.record([this](vk::CommandBuffer cmdbuff) {
            _image->transition_layout(
                vk::ImageLayout::eUndefined,
                vk::ImageLayout::eTransferDstOptimal,
                cmdbuff,
                _image->aspect_flags(),
                0,
                _image->mip_levels());
        });

        // Mip0 data
        const auto mip0_data = ien::conditional_init<std::vector<std::byte>>(
            is_compressed_format(args.format),
            [&image = *args.pimage, format = args.format] {
                return create_compressed_texture_data(image, get_format_compression_type(format));
            },
            [&image = *args.pimage] {
                std::vector<std::byte> result(image.size());
                std::memcpy(result.data(), image.data(), image.size());
                return result;
            });

        // Mip1..n data
        std::vector<std::vector<std::byte>> mipmaps_data;
        if (_image->mip_levels() > 1)
        {
            for (const auto& mip : create_image_mips(*args.pimage, args.mipgen_filter, _image->mip_levels() - 1))
            {
                if (is_compressed_format(args.format))
                {
                    mipmaps_data.push_back(create_compressed_texture_data(mip, get_format_compression_type(args.format)));
                }
                else
                {
                    auto& vec = mipmaps_data.emplace_back();
                    vec.resize(mip.size());
                    std::memcpy(vec.data(), mip.data(), vec.size());
                }
            }
        }

        // Upload mip0
        queue.update_image(*_image, mip0_data.data(), static_cast<uint32_t>(mip0_data.size()), 0);

        // Upload mip1..n
        for (size_t i = 0; i < mipmaps_data.size(); ++i)
        {
            queue.update_image(*_image, mipmaps_data[i].data(), static_cast<uint32_t>(mipmaps_data[i].size()), i + 1);
        }

        // Transition mips to shader-readonly layout
        queue.record([this](vk::CommandBuffer cmdbuff) {
            _image->transition_layout(
                vk::ImageLayout::eTransferDstOptimal,
                vk::ImageLayout::eShaderReadOnlyOptimal,
                cmdbuff,
                _image->aspect_flags(),
                0,
                _image->mip_levels());
        });
    }
} // namespace cathedral::engine