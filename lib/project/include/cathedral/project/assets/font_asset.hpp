#pragma once

#include <cathedral/engine/font.hpp>
#include <cathedral/project/asset.hpp>

namespace cereal
{
    template <typename Archive>
    void CEREAL_SERIALIZE_FUNCTION_NAME(Archive& ar, cathedral::engine::font_glyph_rect& rect)
    {
        ar(make_nvp("offset", rect.offset), make_nvp("size", rect.size));
    }
} // namespace cereal

namespace cathedral::project
{
    class font_asset : public asset
    {
    public:
        using asset::asset;

        CATHEDRAL_ASSET_SUBCLASS_DECL

        void set_glyph_boundind_box(glm::ivec2 size);
        glm::ivec2 glyph_boundind_box() const;

        void set_glyph_rects(std::vector<engine::font_glyph_rect> rects);
        std::vector<engine::font_glyph_rect> glyph_rects() const;

        void set_char_offset(uint32_t offset);
        uint32_t char_offset() const;

        void set_atlas_size(glm::uvec2 size);
        glm::uvec2 atlas_size() const;

        void save_atlas(const ien::image& image) const;
        ien::image load_atlas() const;

    private:
        glm::ivec2 _glyph_bounding_box_size;
        std::vector<engine::font_glyph_rect> _glyph_rects;
        uint32_t _char_offset = 0;
        glm::uvec2 _atlas_size;

        template <class Archive>
        void CEREAL_SERIALIZE_FUNCTION_NAME(Archive& ar)
        {
            ar(cereal::make_nvp("glyph_bounding_box", _glyph_bounding_box_size),
               cereal::make_nvp("glyph_rects", _glyph_rects),
               cereal::make_nvp("char_offset", _char_offset),
               cereal::make_nvp("atlas_size", _atlas_size));
        }
    };
} // namespace cathedral::project