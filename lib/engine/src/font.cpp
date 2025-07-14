#include <cathedral/engine/font.hpp>

#include <ien/image/image.hpp>

#include <cathedral/bits/error.hpp>
#include <ien/io_utils.hpp>

#include <stb_truetype.h>

#include <numeric>
#include <ranges>

namespace cathedral::engine
{
    constexpr int CHAR_COLS = 16;
    constexpr int CHAR_ROWS = 16;
    constexpr int CHAR_COUNT = CHAR_COLS * CHAR_ROWS;

    font::font(const std::string& path, const int glyph_height)
        : _glyph_height(glyph_height)
    {
        const auto font_binary = ien::read_file_binary<unsigned char>(path);
        if (!font_binary)
        {
            CRITICAL_ERROR("Failure reading font");
        }

        stbtt_fontinfo font_info;
        if (stbtt_InitFont(&font_info, font_binary->data(), 0) == 0)
        {
            CRITICAL_ERROR("Failure initializing font");
        }

        const auto image_width = CHAR_COLS * glyph_height;
        const auto image_height = CHAR_ROWS * glyph_height;

        _font_atlas = std::make_unique<ien::image>(image_width, image_height, ien::image_format::R);

        // Fill background with checkerboard pattern for debugging
        for (int y = 0; y < image_height; ++y)
        {
            for (int x = 0; x < image_width; ++x)
            {
                if (y % 8 < 4)
                {
                    _font_atlas->data()[(y * image_height) + x] = x % 8 < 4 ? 255 : 0;
                }
                else
                {
                    _font_atlas->data()[(y * image_height) + x] = x % 8 < 4 ? 0 : 255;
                }
            }
        }

        int max_row_y = 0;
        int image_offset_x = 0;
        int image_offset_y = 0;
        const float scale = stbtt_ScaleForPixelHeight(&font_info, static_cast<float>(glyph_height));
        for (const int ch : std::ranges::iota_view(0, CHAR_COUNT))
        {
            int width;
            int height;
            int xoff;
            int yoff;
            const auto* bitmap = stbtt_GetCodepointBitmap(&font_info, scale, scale, ch, &width, &height, &xoff, &yoff);
            if (image_offset_x + width > static_cast<int>(_font_atlas->width()))
            {
                image_offset_y += max_row_y;
                image_offset_x = 0;
                max_row_y = 0;
            }

            for (int y = 0; y < height; ++y)
            {
                for (int x = 0; x < width; ++x)
                {
                    *_font_atlas->data(image_offset_x + x, image_offset_y + y) = bitmap[(y * width) + x];
                }
            }

            image_offset_x += glyph_height;
            max_row_y = std::max(height, max_row_y);
            _glyph_rects.emplace_back(width, height);
        }
    }

    glm::ivec2 font::glyph_size(const char ch) const
    {
        return _glyph_rects[ch];
    }

    const ien::image& font::atlas() const
    {
        return *_font_atlas;
    }
} // namespace cathedral::engine