#include <cathedral/engine/atlas.hpp>

namespace cathedral::engine
{
    atlas::atlas(atlas_args args)
        : _texture_name(std::move(args.texture_name))
        , _glyph_bounding_box(args.glyph_bounding_box)
        , _glyph_rects(std::move(args.glyph_rects))
    {
    }

    const std::optional<std::string>& atlas::texture_name() const
    {
        return _texture_name;
    }

    glm::ivec2 atlas::glyph_bounding_box() const
    {
        return _glyph_bounding_box;
    }

    const std::vector<glm::ivec2>& atlas::glyph_rects() const
    {
        return _glyph_rects;
    }

    void atlas::set_texture_name(std::optional<std::string> texture_name)
    {
        _texture_name = std::move(texture_name);
    }

    void atlas::set_glyph_bounding_box(const glm::ivec2 glyph_bounding_box)
    {
        _glyph_bounding_box = glyph_bounding_box;
    }

    void atlas::set_glyph_rects(std::vector<glm::ivec2> glyph_rects)
    {
        _glyph_rects = std::move(glyph_rects);
    }
} // namespace cathedral::engine