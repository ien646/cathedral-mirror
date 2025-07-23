#include <cathedral/project/assets/atlas_asset.hpp>

#include <cathedral/project/project.hpp>

namespace cathedral::project
{
    CATHEDRAL_ASSET_SUBCLASS_IMPL(atlas_asset);

    void atlas_asset::set_texture_ref(std::optional<std::string> texture_name)
    {
        _texture_ref = std::move(texture_name);
    }

    const std::optional<std::string>& atlas_asset::texture_ref() const
    {
        return _texture_ref;
    }

    void atlas_asset::set_glyph_bounding_box(const glm::ivec2 glyph_bounding_box)
    {
        _glyph_bounding_box = glyph_bounding_box;
    }

    glm::ivec2 atlas_asset::glyph_bounding_box() const
    {
        return _glyph_bounding_box;
    }

    void atlas_asset::set_glyph_rects(std::vector<glm::ivec2> glyph_rects)
    {
        _glyph_rects = std::move(glyph_rects);
    }

    const std::vector<glm::ivec2>& atlas_asset::glyph_rects() const
    {
        return _glyph_rects;
    }
} // namespace cathedral::project