#pragma once

#include <cathedral/core.hpp>
#include <cathedral/engine/renderer.hpp>
#include <cathedral/engine/texture.hpp>

#include <ien/image/image.hpp>

#include <glm/vec2.hpp>

#include <memory>
#include <string>
#include <unordered_map>

FORWARD_CLASS(cathedral::engine, renderer);

namespace cathedral::engine
{
    struct font_glyph_rect
    {
        CATHEDRAL_ALIGNED_UNIFORM(glm::ivec2, offset);
        CATHEDRAL_ALIGNED_UNIFORM(glm::ivec2, size);
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

    class font
    {
    public:
        font(
            std::string name,
            const ien::image& src_image,
            glm::uvec2 glyph_bbox_size,
            std::vector<font_glyph_rect> glyph_rects,
            int char_offset,
            renderer& renderer);

        font(
            std::string name,
            std::shared_ptr<texture> texture,
            glm::uvec2 glyph_bbox_size,
            std::vector<font_glyph_rect> glyph_rects,
            int char_offset);

        const texture& atlas_texture() const;
        glm::uvec2 glyph_bbox_size() const;
        const std::vector<font_glyph_rect>& glyph_rects() const;
        int char_offset() const;

    private:
        std::string _name;
        std::shared_ptr<texture> _texture;
        glm::uvec2 _glyph_bbox_size;
        std::vector<font_glyph_rect> _glyph_rects;
        int _char_offset;
    };
} // namespace cathedral::engine