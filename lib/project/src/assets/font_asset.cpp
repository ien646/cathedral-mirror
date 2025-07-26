#include "cathedral/compression.hpp"

#include <cathedral/project/assets/font_asset.hpp>

#include <cathedral/project/project.hpp>

namespace cathedral::project
{
    CATHEDRAL_ASSET_SUBCLASS_IMPL(font_asset);

    void font_asset::set_glyph_boundind_box(const glm::ivec2 size)
    {
        _glyph_bounding_box_size = size;
    }

    glm::ivec2 font_asset::glyph_boundind_box() const
    {
        return _glyph_bounding_box_size;
    }

    void font_asset::set_glyph_rects(std::vector<engine::font_glyph_rect> rects)
    {
        _glyph_rects = std::move(rects);
    }

    std::vector<engine::font_glyph_rect> font_asset::glyph_rects() const
    {
        return _glyph_rects;
    }

    void font_asset::set_char_offset(const uint32_t offset)
    {
        _char_offset = offset;
    }

    uint32_t font_asset::char_offset() const
    {
        return _char_offset;
    }

    void font_asset::set_atlas_size(const glm::uvec2 size)
    {
        _atlas_size = size;
    }

    glm::uvec2 font_asset::atlas_size() const
    {
        return _atlas_size;
    }

    void font_asset::save_atlas(const ien::image& image) const
    {
        CRITICAL_CHECK(image.format() == ien::image_format::R, "Font atlas image format must be grayscale (1 channel)");

        const auto compressed_data =
            compress_data(std::span{ reinterpret_cast<const std::byte*>(image.data()), image.size() });

        const auto target_path = bin_path();

        if (std::filesystem::exists(target_path))
        {
            std::filesystem::remove(target_path);
        }

        const bool write_ok = ien::write_file_binary(target_path, compressed_data);
        if (!write_ok)
        {
            CRITICAL_ERROR("Failed to write font atlas file");
        }
    }

    ien::image font_asset::load_atlas() const
    {
        const auto path = bin_path();
        if (!std::filesystem::exists(path))
        {
            CRITICAL_ERROR("Font atlas file does not exist");
        }

        const auto compressed_data = ien::read_file_binary(path);
        if (!compressed_data)
        {
            CRITICAL_ERROR("Failed to read font atlas file");
        }

        const size_t uncompressed_size = _atlas_size.x * _atlas_size.y;
        const auto uncompressed_data = decompress_data(*compressed_data, uncompressed_size);

        ien::image result(_atlas_size.x, _atlas_size.y, ien::image_format::R);
        std::memcpy(result.data(), uncompressed_data.data(), uncompressed_size);

        return result;
    }
} // namespace cathedral::project