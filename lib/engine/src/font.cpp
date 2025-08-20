#include <cathedral/engine/font.hpp>

#include <ien/image/image.hpp>

#include <cathedral/bits/error.hpp>
#include <ien/io_utils.hpp>

#include <stb_truetype.h>

#include <memory>
#include <numeric>
#include <ranges>
#include <utility>

namespace cathedral::engine
{
    font_data generate_font_data(
        const std::string& ttf_font_path,
        const uint32_t glyph_height,
        const glm::uvec2 atlas_size,
        int char_gen_offset)
    {
        font_data result;

        const auto font_binary = ien::read_file_binary<unsigned char>(ttf_font_path);
        if (!font_binary)
        {
            CRITICAL_ERROR("Failure reading font");
        }

        stbtt_fontinfo font_info;
        if (stbtt_InitFont(&font_info, font_binary->data(), 0) == 0)
        {
            CRITICAL_ERROR("Failure initializing font");
        }

        const auto cols = atlas_size.x / glyph_height;
        const auto rows = atlas_size.y / glyph_height;

        const auto char_count = cols * rows;

        result.atlas_image = std::make_unique<ien::image>(atlas_size.x, atlas_size.y, ien::image_format::R);

        // Fill background with black
        std::memset(result.atlas_image->data(), 0, result.atlas_image->size());

        uint32_t image_offset_x = 0;
        uint32_t image_offset_y = 0;
        const float scale = stbtt_ScaleForPixelHeight(&font_info, static_cast<float>(glyph_height));
        for (const int ch : std::ranges::iota_view(char_gen_offset, static_cast<int>(char_count)))
        {
            int width;
            int height;
            int xoff;
            int yoff;
            auto* bitmap = stbtt_GetCodepointBitmap(&font_info, scale, scale, ch, &width, &height, &xoff, &yoff);
            if (std::cmp_greater(image_offset_x + glyph_height, static_cast<int>(result.atlas_image->width())))
            {
                image_offset_y += glyph_height;
                image_offset_x = 0;
            }

            for (int y = 0; y < height; ++y)
            {
                auto* row_ptr = result.atlas_image->data(image_offset_x, image_offset_y + y);
                for (int x = 0; x < width; ++x)
                {
                    row_ptr[x] = bitmap[(y * width) + x];
                }
            }
            stbtt_FreeBitmap(bitmap, nullptr);

            image_offset_x += glyph_height;

            int hadvance = 0;
            int lbearing = 0;
            stbtt_GetCodepointHMetrics(&font_info, ch, &hadvance, &lbearing);

            const font_glyph_info info{ .offset = { xoff, yoff },
                                        .size = { width, height },
                                        .horizontal_advance = static_cast<float>(hadvance) * scale,
                                        .left_bearing = static_cast<float>(lbearing) * scale };
            result.glyph_infos.emplace_back(info);

            result.kerning_table.emplace_back();
            for (const int ch_other : std::ranges::iota_view(char_gen_offset, static_cast<int>(char_count)))
            {
                const int kerning = stbtt_GetCodepointKernAdvance(&font_info, ch, ch_other);
                result.kerning_table.back().push_back(static_cast<float>(kerning) * scale);
            }
        }

        result.glyph_bounding_box_size = { glyph_height, glyph_height };

        return result;
    }

    font::font(
        std::string name,
        const ien::image& src_image,
        const glm::uvec2 glyph_bbox_size,
        std::vector<font_glyph_info> glyph_rects,
        const int char_offset,
        std::vector<std::vector<float>> kerning_table,
        renderer& renderer)
        : _name(std::move(name))
        , _glyph_bbox_size(glyph_bbox_size)
        , _glyph_rects(std::move(glyph_rects))
        , _char_offset(char_offset)
        , _kerning_table(std::move(kerning_table))
    {
        const auto font_texture_name = "__font_texture:" + _name;
        if (renderer.textures().contains(font_texture_name))
        {
            _texture = renderer.textures().at(font_texture_name);
        }
        else
        {
            _texture = renderer.create_color_texture(
                font_texture_name,
                src_image,
                8,
                vk::Filter::eLinear,
                vk::Filter::eLinear,
                ien::resize_filter::POINT_SAMPLE);
        }
    }

    font::font(
        std::string name,
        std::shared_ptr<texture> texture,
        const glm::uvec2 glyph_bbox_size,
        std::vector<font_glyph_info> glyph_rects,
        const int char_offset,
        std::vector<std::vector<float>> kerning_table)
        : _name(std::move(name))
        , _texture(std::move(texture))
        , _glyph_bbox_size(glyph_bbox_size)
        , _glyph_rects(std::move(glyph_rects))
        , _char_offset(char_offset)
        , _kerning_table(std::move(kerning_table))
    {
    }

    std::shared_ptr<texture> font::atlas_texture() const
    {
        return _texture;
    }

    glm::uvec2 font::glyph_bbox_size() const
    {
        return _glyph_bbox_size;
    }

    const std::vector<font_glyph_info>& font::glyph_infos() const
    {
        return _glyph_rects;
    }

    int font::char_offset() const
    {
        return _char_offset;
    }

    float font::get_char_kerning(const uint32_t from, const uint32_t to) const
    {
        return _kerning_table[from][to];
    }
} // namespace cathedral::engine