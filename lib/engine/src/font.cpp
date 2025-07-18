#include <cathedral/engine/font.hpp>

#include <ien/image/image.hpp>

#include <cathedral/bits/error.hpp>
#include <ien/io_utils.hpp>

#include <stb_truetype.h>

#include <numeric>
#include <ranges>

namespace cathedral::engine
{
    font::font(const font_args& args)
        : _atlas_size(args.atlas_size)
        , _char_offset(args.char_gen_offset)
        , _glyph_height(args.char_box_size)
    {
    }

    glm::ivec2 font::glyph_size(const char ch) const
    {
        return _glyph_rects[ch];
    }

    atlas font::generate_atlas(const std::string& font_path) const
    {
        atlas result;

        const auto font_binary = ien::read_file_binary<unsigned char>(font_path);
        if (!font_binary)
        {
            CRITICAL_ERROR("Failure reading font");
        }

        stbtt_fontinfo font_info;
        if (stbtt_InitFont(&font_info, font_binary->data(), 0) == 0)
        {
            CRITICAL_ERROR("Failure initializing font");
        }

        const auto char_count = cols() * rows();

        const auto image_width = cols() * _glyph_height;
        const auto image_height = rows() * _glyph_height;

        result.image = std::make_unique<ien::image>(image_width, image_height, ien::image_format::R);

        // Fill background with checkerboard pattern for debugging
        for (int y = 0; y < image_height; ++y)
        {
            for (int x = 0; x < image_width; ++x)
            {
                if (y % 8 < 4)
                {
                    result.image->data()[(y * image_height) + x] = x % 8 < 4 ? 255 : 0;
                }
                else
                {
                    result.image->data()[(y * image_height) + x] = x % 8 < 4 ? 0 : 255;
                }
            }
        }

        int max_row_y = 0;
        int image_offset_x = 0;
        int image_offset_y = 0;
        const float scale = stbtt_ScaleForPixelHeight(&font_info, static_cast<float>(_glyph_height));
        for (const int ch : std::ranges::iota_view(_char_offset, char_count))
        {
            int width;
            int height;
            int xoff;
            int yoff;
            const auto* bitmap = stbtt_GetCodepointBitmap(&font_info, scale, scale, ch, &width, &height, &xoff, &yoff);
            if (image_offset_x + width > static_cast<int>(result.image->width()))
            {
                image_offset_y += max_row_y;
                image_offset_x = 0;
                max_row_y = 0;
            }

            for (int y = 0; y < height; ++y)
            {
                auto* row_ptr = result.image->data(image_offset_x, image_offset_y + y);
                for (int x = 0; x < width; ++x)
                {
                    row_ptr[x] = bitmap[(y * width) + x];
                }
            }

            image_offset_x += _glyph_height;
            max_row_y = std::max(height, max_row_y);
            result.glyph_rects.emplace_back(width, height);
        }

        return result;
    }

    int font::cols() const
    {
        return _atlas_size.x / _glyph_height;
    }

    int font::rows() const
    {
        return _atlas_size.y / _glyph_height;
    }
} // namespace cathedral::engine