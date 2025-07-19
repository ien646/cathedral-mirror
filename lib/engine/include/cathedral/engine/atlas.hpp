#pragma once

#include <cathedral/core.hpp>

#include <glm/vec2.hpp>

namespace cathedral::engine
{
    struct atlas_args
    {
        std::string texture_name;
        glm::ivec2 glyph_bounding_box;
        std::vector<glm::ivec2> glyph_rects;
    };

    class atlas
    {
    public:
        explicit atlas(atlas_args args);

        const std::string& texture_name() const;
        glm::ivec2 glyph_bounding_box() const;
        const std::vector<glm::ivec2>& glyph_rects() const;

        void set_texture_name(std::string texture_name);
        void set_glyph_bounding_box(glm::ivec2 glyph_bounding_box);
        void set_glyph_rects(std::vector<glm::ivec2> glyph_rects);

    private:
        std::string _texture_name;
        glm::ivec2 _glyph_bounding_box;
        std::vector<glm::ivec2> _glyph_rects;
    };
} // namespace cathedral::engine