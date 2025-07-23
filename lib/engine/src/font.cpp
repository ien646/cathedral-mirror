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
    font::font(std::string font_path)
        : _font_path(std::move(font_path))
    {
    }

    std::pair<std::unique_ptr<ien::image>, atlas_args> font::generate_image_and_atlas_args(const atlas_image_args& args) const
    {
        std::pair<std::unique_ptr<ien::image>, atlas_args> result;

        auto& [result_image, result_atlas_args] = result;

        const auto font_binary = ien::read_file_binary<unsigned char>(_font_path);
        if (!font_binary)
        {
            CRITICAL_ERROR("Failure reading font");
        }

        stbtt_fontinfo font_info;
        if (stbtt_InitFont(&font_info, font_binary->data(), 0) == 0)
        {
            CRITICAL_ERROR("Failure initializing font");
        }

        const auto cols = args.atlas_size.x / args.char_box_size;
        const auto rows = args.atlas_size.y / args.char_box_size;

        const auto char_count = cols * rows;

        result_image = std::make_unique<ien::image>(args.atlas_size.x, args.atlas_size.y, ien::image_format::R);

        // Fill background with checkerboard pattern for debugging
        for (int y = 0; y < args.atlas_size.y; ++y)
        {
            for (int x = 0; x < args.atlas_size.x; ++x)
            {
                if (y % 8 < 4)
                {
                    result_image->data()[(y * args.atlas_size.y) + x] = x % 8 < 4 ? 255 : 0;
                }
                else
                {
                    result_image->data()[(y * args.atlas_size.y) + x] = x % 8 < 4 ? 0 : 255;
                }
            }
        }

        int max_row_y = 0;
        int image_offset_x = 0;
        int image_offset_y = 0;
        const float scale = stbtt_ScaleForPixelHeight(&font_info, static_cast<float>(args.char_box_size));
        for (const int ch : std::ranges::iota_view(args.char_gen_offset, char_count))
        {
            int width;
            int height;
            int xoff;
            int yoff;
            const auto* bitmap = stbtt_GetCodepointBitmap(&font_info, scale, scale, ch, &width, &height, &xoff, &yoff);
            if (image_offset_x + width > static_cast<int>(result_image->width()))
            {
                image_offset_y += max_row_y;
                image_offset_x = 0;
                max_row_y = 0;
            }

            for (int y = 0; y < height; ++y)
            {
                auto* row_ptr = result_image->data(image_offset_x, image_offset_y + y);
                for (int x = 0; x < width; ++x)
                {
                    row_ptr[x] = bitmap[(y * width) + x];
                }
            }

            image_offset_x += args.char_box_size;
            max_row_y = std::max(height, max_row_y);
            result_atlas_args.glyph_rects.emplace_back(width, height);
        }

        result_atlas_args.glyph_bounding_box = { args.char_box_size, args.char_box_size };

        return result;
    }
} // namespace cathedral::engine