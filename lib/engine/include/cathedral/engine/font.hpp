#pragma once

#include <cathedral/core.hpp>

#include <ien/image/image.hpp>

#include <glm/vec2.hpp>

#include <memory>
#include <string>

namespace cathedral::engine
{
    class font
    {
    public:
        explicit font(const std::string& path, int glyph_height);

        glm::ivec2 glyph_size(char ch) const;

        const ien::image& atlas() const;

    private:
        std::unique_ptr<ien::image> _font_atlas;
        int _glyph_height = 0;
        std::vector<glm::ivec2> _glyph_rects;
    };
} // namespace cathedral::engine