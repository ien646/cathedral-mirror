#pragma once

#include <cathedral/engine/atlas.hpp>
#include <cathedral/project/asset.hpp>

namespace cathedral::project
{
    class atlas_asset final : public asset
    {
    public:
        using asset::asset;

        CATHEDRAL_ASSET_SUBCLASS_DECL

        void set_texture_ref(std::string texture_name);
        const std::string& texture_ref() const;
        void set_glyph_bounding_box(glm::ivec2 glyph_bounding_box);
        glm::ivec2 glyph_bounding_box() const;
        void set_glyph_rects(std::vector<glm::ivec2> glyph_rects);
        const std::vector<glm::ivec2>& glyph_rects() const;

        constexpr const char* typestr() const override { return "atlas"; }

    private:
        std::string _texture_ref;
        glm::ivec2 _glyph_bounding_box;
        std::vector<glm::ivec2> _glyph_rects;

        friend class cereal::access;

        template <typename Archive>
        void CEREAL_SERIALIZE_FUNCTION_NAME(Archive& ar)
        {
            ar(cereal::make_nvp("asset", cereal::base_class<asset>(this)),
               cereal::make_nvp("texture_ref", _texture_ref),
               cereal::make_nvp("glyph_bounding_box", _glyph_bounding_box),
               cereal::make_nvp("glyph_rects", _glyph_rects));
        }
    };
} // namespace cathedral::project