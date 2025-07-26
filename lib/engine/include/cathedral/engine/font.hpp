#pragma once

#include <cathedral/core.hpp>

#include <ien/image/image.hpp>

#include <glm/vec2.hpp>

#include <memory>
#include <string>

namespace cathedral::engine
{
    struct font_glyph_rect
    {
        glm::ivec2 offset;
        glm::ivec2 size;
    };

    struct font_data
    {
        std::unique_ptr<ien::image> atlas_image;
        glm::uvec2 glyph_bounding_box_size;
        std::vector<font_glyph_rect> glyph_rects;
        uint32_t char_offset = 0;
    };

    font_data generate_font_data(
        const std::string& ttf_font_path,
        uint32_t glyph_height,
        glm::uvec2 atlas_size,
        int char_gen_offset = 0);
} // namespace cathedral::engine