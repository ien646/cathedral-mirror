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

        // Fill background with checkerboard pattern for debugging
        for (uint32_t y = 0; y < atlas_size.y; ++y)
        {
            for (uint32_t x = 0; x < atlas_size.x; ++x)
            {
                if (y % 8 < 4)
                {
                    result.atlas_image->data()[(y * atlas_size.y) + x] = x % 8 < 4 ? 255 : 0;
                }
                else
                {
                    result.atlas_image->data()[(y * atlas_size.y) + x] = x % 8 < 4 ? 0 : 255;
                }
            }
        }

        int max_row_y = 0;
        uint32_t image_offset_x = 0;
        uint32_t image_offset_y = 0;
        const float scale = stbtt_ScaleForPixelHeight(&font_info, static_cast<float>(glyph_height));
        for (const int ch : std::ranges::iota_view(char_gen_offset, static_cast<int>(char_count)))
        {
            int width;
            int height;
            int xoff;
            int yoff;
            const auto* bitmap = stbtt_GetCodepointBitmap(&font_info, scale, scale, ch, &width, &height, &xoff, &yoff);
            if (std::cmp_greater(image_offset_x + width, static_cast<int>(result.atlas_image->width())))
            {
                image_offset_y += max_row_y;
                image_offset_x = 0;
                max_row_y = 0;
            }

            for (int y = yoff; y < height; ++y)
            {
                auto* row_ptr = result.atlas_image->data(image_offset_x, image_offset_y + y);
                for (int x = xoff; x < width; ++x)
                {
                    row_ptr[x] = bitmap[(y * width) + x];
                }
            }

            image_offset_x += glyph_height;
            max_row_y = std::max(height, max_row_y);

            const font_glyph_rect rect{ .offset = { xoff, yoff }, .size = { width, height } };
            result.glyph_rects.emplace_back(rect);
        }

        result.glyph_bounding_box_size = { glyph_height, glyph_height };

        return result;
    }
} // namespace cathedral::engine