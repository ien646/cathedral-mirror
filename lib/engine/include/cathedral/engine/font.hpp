#pragma once

#include <cathedral/core.hpp>

#include <ien/image/image.hpp>

#include <glm/vec2.hpp>

#include <memory>
#include <string>

namespace cathedral::engine
{
    struct font_args
    {
        glm::ivec2 atlas_size = { 1024, 1024 };
        int char_box_size = 64;
        int char_gen_offset = 0;
    };

    struct atlas
    {
        std::unique_ptr<ien::image> image;
        std::vector<glm::ivec2> glyph_rects;
        int char_box_size = 64;
        int char_gen_offset = 0;
    };

    class font
    {
    public:
        explicit font(const font_args& args);

        glm::ivec2 glyph_size(char ch) const;

        atlas generate_atlas(const std::string& font_path) const;

        int cols() const;
        int rows() const;

    private:
        std::string _path;
        glm::ivec2 _atlas_size;
        int _char_offset;
        int _glyph_height;
        std::vector<glm::ivec2> _glyph_rects;
    };
} // namespace cathedral::engine