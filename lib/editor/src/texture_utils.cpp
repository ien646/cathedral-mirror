#include <cathedral/editor/texture_utils.hpp>

#include <cathedral/engine/default_resources.hpp>
#include <cathedral/engine/texture_decompression.hpp>

#include <QImage>

namespace cathedral::editor
{
    constexpr uint32_t qRgba(uint8_t r, uint8_t g, uint8_t b, uint8_t a)
    {
        return (static_cast<uint16_t>(a) << 24) | (static_cast<uint16_t>(r) << 16) | (static_cast<uint16_t>(g) << 8) |
               static_cast<uint16_t>(b);
    }

    std::vector<std::byte, ien::aligned_allocator<std::byte, 4>> rgba_to_qrgba(std::span<const std::byte> image_data)
    {
        const auto pixel_count = image_data.size() / 4;
        std::vector<std::byte, ien::aligned_allocator<std::byte, 4>> rgba_data(pixel_count * 4);
        auto* rgba_u32ptr = reinterpret_cast<uint32_t*>(rgba_data.data());

        for (size_t i = 0; i < pixel_count; ++i)
        {
            size_t src_offset = i * 4;
            rgba_u32ptr[i] = qRgba(
                static_cast<uint8_t>(image_data[src_offset + 2]),
                static_cast<uint8_t>(image_data[src_offset + 1]),
                static_cast<uint8_t>(image_data[src_offset + 0]),
                static_cast<uint8_t>(image_data[src_offset + 3]));
        }
        return rgba_data;
    }

    std::vector<std::byte, ien::aligned_allocator<std::byte, 4>> rgb_to_qrgba(std::span<const std::byte> image_data)
    {
        const auto pixel_count = image_data.size() / 3;
        std::vector<std::byte, ien::aligned_allocator<std::byte, 4>> rgba_data(pixel_count * 4);
        auto* rgba_u32ptr = reinterpret_cast<uint32_t*>(rgba_data.data());
        for (size_t i = 0; i < pixel_count; ++i)
        {
            size_t src_offset = i * 3;
            rgba_u32ptr[i] = editor::qRgba(
                static_cast<uint8_t>(image_data[src_offset + 2]),
                static_cast<uint8_t>(image_data[src_offset + 1]),
                static_cast<uint8_t>(image_data[src_offset]),
                255);
        }
        return rgba_data;
    }

    std::vector<std::byte, ien::aligned_allocator<std::byte, 4>> rg_to_qrgba(std::span<const std::byte> image_data)
    {
        const auto pixel_count = image_data.size() / 2;
        std::vector<std::byte, ien::aligned_allocator<std::byte, 4>> rgba_data(pixel_count * 4);
        auto* rgba_u32ptr = reinterpret_cast<uint32_t*>(rgba_data.data());
        for (size_t i = 0; i < pixel_count; ++i)
        {
            size_t src_offset = i * 2;
            rgba_u32ptr[i] = editor::qRgba(
                0,
                static_cast<uint8_t>(image_data[src_offset + 1]),
                static_cast<uint8_t>(image_data[src_offset]),
                255);
        }
        return rgba_data;
    }

    std::vector<std::byte, ien::aligned_allocator<std::byte, 4>> r_to_qrgba(std::span<const std::byte> image_data)
    {
        std::vector<std::byte, ien::aligned_allocator<std::byte, 4>> rgba_data(image_data.size() * 4);
        auto* rgba_u32ptr = reinterpret_cast<uint32_t*>(rgba_data.data());
        for (size_t i = 0; i < image_data.size(); ++i)
        {
            rgba_u32ptr[i] = editor::qRgba(
                static_cast<uint8_t>(image_data[i]),
                static_cast<uint8_t>(image_data[i]),
                static_cast<uint8_t>(image_data[i]),
                255);
        }
        return rgba_data;
    }

    std::vector<std::byte, ien::aligned_allocator<std::byte, 4>> image_data_to_qrgba(
        std::span<const std::byte> image_data,
        engine::texture_format format)
    {
        using enum engine::texture_format;
        switch (format)
        {
        case R8G8B8A8_SRGB:
        case R8G8B8A8_LINEAR:
        case DXT1_BC1_LINEAR:
        case DXT1_BC1_SRGB:
        case DXT5_BC3_LINEAR:
        case DXT5_BC3_SRGB:
            return rgba_to_qrgba(image_data);
        case R8G8B8_SRGB:
        case R8G8B8_LINEAR:
            return rgb_to_qrgba(image_data);
        case R8G8_SRGB:
        case R8G8_LINEAR:
            return rg_to_qrgba(image_data);
        case R8_SRGB:
        case R8_LINEAR:
            return r_to_qrgba(image_data);
        default:
            CRITICAL_ERROR("Unhandled texture format");
        }
    }

    const QImage _default_image = [] {
        const auto& default_image_source = engine::get_default_texture_image();
        return QImage(
            default_image_source.data(),
            static_cast<int>(default_image_source.width()),
            static_cast<int>(default_image_source.height()),
            QImage::Format_RGBA8888);
    }();

    const QImage& get_default_texture_qimage()
    {
        return _default_image;
    }

    QImage mip_to_qimage(std::span<const std::byte> data, uint32_t width, uint32_t height, engine::texture_format format)
    {
        const std::vector<std::byte, ien::aligned_allocator<std::byte, 4>> image_data =
            [&] -> std::vector<std::byte, ien::aligned_allocator<std::byte, 4>> {
            if (engine::is_compressed_format(format))
            {
                auto tex_data = engine::decompress_texture_data<ien::aligned_allocator<std::byte, 4>>(
                    data.data(),
                    width,
                    height,
                    engine::get_format_compression_type(format));
                return { tex_data.begin(), tex_data.end() };
            }
            else
            {
                return { data.begin(), data.end() };
            }
        }();

        const auto rgba_data = image_data_to_qrgba(image_data, format);
        QImage result(width, height, QImage::Format::Format_RGBA8888);

        CRITICAL_CHECK(result.sizeInBytes() == static_cast<qsizetype>(rgba_data.size()));

        std::memcpy(result.bits(), rgba_data.data(), rgba_data.size());
        return result;
    };
} // namespace cathedral::editor