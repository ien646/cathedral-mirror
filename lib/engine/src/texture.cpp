#include <cathedral/engine/texture.hpp>

#include <cathedral/engine/upload_queue.hpp>

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

    constexpr bool is_compressed_format(texture_format fmt)
    {
        switch(fmt)
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
        CRITICAL_CHECK(args.mipmap_levels > 0);

        gfx::image_args image_args;
        image_args.vkctx = &queue.vkctx();
        image_args.aspect_flags = args.image_aspect_flags;
        image_args.width = args.pimage->width();
        image_args.height = args.pimage->height();
        image_args.format = tex_fmt_to_vk_fmt(args.format);
        image_args.mipmap_levels = args.mipmap_levels;
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
        imageview_info.format = get_imageview_format(image_args.format);
        imageview_info.subresourceRange.aspectMask = args.image_aspect_flags;
        imageview_info.subresourceRange.baseArrayLayer = 0;
        imageview_info.subresourceRange.baseMipLevel = 0;
        imageview_info.subresourceRange.layerCount = 1;
        imageview_info.subresourceRange.levelCount = _image->mip_levels();

        _imageview = queue.vkctx().device().createImageViewUnique(imageview_info);

        // Transition all mips to transferDst
        queue.record([&](vk::CommandBuffer cmdbuff) {
            _image->transition_layout(
                vk::ImageLayout::eUndefined,
                vk::ImageLayout::eTransferDstOptimal,
                cmdbuff,
                _image->aspect_flags(),
                0,
                _image->mip_levels());
        });

        // Upload image to mip-0
        queue.update_image(*_image, args.pimage->data(), args.pimage->size(), 0);

        // Transition mip-0 to transferSrc
        queue.record([&](vk::CommandBuffer cmdbuff) {
            _image->transition_layout(
                vk::ImageLayout::eTransferDstOptimal,
                vk::ImageLayout::eTransferSrcOptimal,
                cmdbuff,
                _image->aspect_flags(),
                0,
                1);
        });

        // Blit mip-0 to other mip levels
        for (size_t i = 1; i < _image->mip_levels(); ++i)
        {
            // Every subsequent mip level gets its size divided by 2^i (2, 4, 8, 16, ...)
            const uint32_t source_size_divider = std::pow(2, i);

            const uint32_t dst_mip_width = std::max(_image->width() / source_size_divider, 1u);
            const uint32_t dst_mip_height = std::max(_image->height() / source_size_divider, 1u);

            vk::ImageBlit blit;
            blit.srcOffsets[0] = vk::Offset3D(0, 0, 0);
            blit.srcOffsets[1] = vk::Offset3D(_image->width(), _image->height(), 1);
            blit.dstOffsets[0] = vk::Offset3D(0, 0, 0);
            blit.dstOffsets[1] = vk::Offset3D(dst_mip_width, dst_mip_height, 1);
            blit.srcSubresource.aspectMask = _image->aspect_flags();
            blit.srcSubresource.baseArrayLayer = 0;
            blit.srcSubresource.layerCount = 1;
            blit.srcSubresource.mipLevel = 0;
            blit.dstSubresource.aspectMask = _image->aspect_flags();
            blit.dstSubresource.mipLevel = i;
            blit.dstSubresource.baseArrayLayer = 0;
            blit.dstSubresource.layerCount = 1;

            queue.record([&](vk::CommandBuffer cmdbuff) {
                cmdbuff.blitImage(
                    _image->get_image(),
                    vk::ImageLayout::eTransferSrcOptimal,
                    _image->get_image(),
                    vk::ImageLayout::eTransferDstOptimal,
                    blit,
                    args.mipmap_generation_filter);
            });
        }

        // Transition mips to shader-readonly layout
        queue.record([&](vk::CommandBuffer cmdbuff) {
            // mip0
            _image->transition_layout(
                vk::ImageLayout::eTransferSrcOptimal,
                vk::ImageLayout::eShaderReadOnlyOptimal,
                cmdbuff,
                _image->aspect_flags(),
                0,
                1);

            // mip1 ... mipn
            if (_image->mip_levels() > 1)
            {
                _image->transition_layout(
                    vk::ImageLayout::eTransferDstOptimal,
                    vk::ImageLayout::eShaderReadOnlyOptimal,
                    cmdbuff,
                    _image->aspect_flags(),
                    1,
                    _image->mip_levels() - 1);
            }
        });
    }
} // namespace cathedral::engine