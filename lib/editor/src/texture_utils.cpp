#include <cathedral/editor/texture_utils.hpp>

#include <cathedral/engine/default_resources.hpp>

#include <QImage>
#include <QRgb>

namespace cathedral::editor
{
    std::vector<uint8_t> rgba_to_qrgba(const std::vector<uint8_t>& image_data)
    {
        const uint32_t pixel_count = image_data.size() / 4;
        std::vector<uint8_t> rgba_data(pixel_count * 4);
        auto* rgba_u32ptr = reinterpret_cast<QRgb*>(rgba_data.data());

        for (size_t i = 0; i < pixel_count; ++i)
        {
            size_t src_offset = i * 4;
            rgba_u32ptr[i] = qRgba(
                image_data[src_offset + 2],
                image_data[src_offset + 1],
                image_data[src_offset + 0],
                image_data[src_offset + 3]);
        }
        return rgba_data;
    }

    std::vector<uint8_t> rgb_to_qrgba(const std::vector<uint8_t>& image_data)
    {
        const uint32_t pixel_count = image_data.size() / 3;
        std::vector<uint8_t> rgba_data(pixel_count * 4);
        auto* rgba_u32ptr = reinterpret_cast<QRgb*>(rgba_data.data());
        for (size_t i = 0; i < pixel_count; ++i)
        {
            size_t src_offset = i * 3;
            rgba_u32ptr[i] = qRgba(image_data[src_offset + 2], image_data[src_offset + 1], image_data[src_offset], 255);
        }
        return rgba_data;
    }

    std::vector<uint8_t> rg_to_qrgba(const std::vector<uint8_t>& image_data)
    {
        const uint32_t pixel_count = image_data.size() / 2;
        std::vector<uint8_t> rgba_data(pixel_count * 4);
        auto* rgba_u32ptr = reinterpret_cast<QRgb*>(rgba_data.data());
        for (size_t i = 0; i < pixel_count; ++i)
        {
            size_t src_offset = i * 2;
            rgba_u32ptr[i] = qRgba(0, image_data[src_offset + 1], image_data[src_offset], 255);
        }
        return rgba_data;
    }

    std::vector<uint8_t> r_to_qrgba(const std::vector<uint8_t>& image_data)
    {
        std::vector<uint8_t> rgba_data(image_data.size() * 4);
        auto* rgba_u32ptr = reinterpret_cast<uint32_t*>(rgba_data.data());
        for (size_t i = 0; i < image_data.size(); ++i)
        {
            rgba_u32ptr[i] = qRgba(image_data[i], image_data[i], image_data[i], 255);
        }
        return rgba_data;
    }

    std::vector<uint8_t> image_data_to_qrgba(const std::vector<uint8_t>& image_data, engine::texture_format format)
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

    const QImage& get_default_texture_qimage()
    {
        static const QImage image = [] {
            const auto& default_image_source = engine::get_default_texture_image();
            return QImage(
                default_image_source.data(),
                default_image_source.width(),
                default_image_source.height(),
                QImage::Format_RGBA8888);
        }();
        return image;
    }
} // namespace cathedral::editor