#pragma once

#include "cathedral/compression.hpp"

#include <cathedral/engine/font.hpp>
#include <cathedral/project/asset.hpp>

#include <cathedral/glm_serializers.hpp>

#include <cereal/cereal.hpp>
#include <cereal/types/utility.hpp>
#include <cereal/types/vector.hpp>

namespace cereal
{
    template <typename Archive>
    void CEREAL_SERIALIZE_FUNCTION_NAME(Archive& ar, cathedral::engine::font_glyph_info& rect)
    {
        ar(make_nvp("offset", rect.offset),
           make_nvp("size", rect.size),
           make_nvp("horizontal_advance", rect.horizontal_advance),
           make_nvp("left_bearing", rect.left_bearing));
    }
} // namespace cereal

namespace cathedral::project
{
    class font_asset final : public asset
    {
    public:
        using asset::asset;

        CATHEDRAL_ASSET_SUBCLASS_DECL

        void set_glyph_boundind_box(glm::ivec2 size);
        glm::ivec2 glyph_boundind_box() const;

        void set_glyph_rects(std::vector<engine::font_glyph_info> rects);
        std::vector<engine::font_glyph_info> glyph_rects() const;

        void set_char_offset(uint32_t offset);
        uint32_t char_offset() const;

        void set_atlas_size(glm::uvec2 size);
        glm::uvec2 atlas_size() const;

        void save_atlas(const ien::image& image) const;
        ien::image load_atlas() const;

        void set_kerning_table(std::vector<float> table);
        std::vector<float> kerning_table() const;

        constexpr const char* typestr() const override { return "font"; }

    private:
        glm::ivec2 _glyph_bounding_box_size;
        std::vector<engine::font_glyph_info> _glyph_rects;
        uint32_t _char_offset = 0;
        glm::uvec2 _atlas_size;
        std::vector<float> _kerning_table;

        friend class cereal::access;

        template <typename Archive>
        void CEREAL_SAVE_FUNCTION_NAME(Archive& ar) const
        {
            std::vector<std::byte> kerning_table_bytes;
            kerning_table_bytes.reserve(_kerning_table.size() * sizeof(float));
            for (const auto& value : std::as_bytes(std::span{ _kerning_table.data(), _kerning_table.size() }))
            {
                kerning_table_bytes.push_back(value);
            }

            std::pair<size_t, std::vector<std::byte>> compressed_table;
            compressed_table.first = kerning_table_bytes.size();
            compressed_table.second = compress_data(kerning_table_bytes);

            ar(cereal::make_nvp("glyph_bounding_box", _glyph_bounding_box_size),
               cereal::make_nvp("glyph_rects", _glyph_rects),
               cereal::make_nvp("char_offset", _char_offset),
               cereal::make_nvp("atlas_size", _atlas_size),
               cereal::make_nvp("kerning_table", compressed_table));
        }

        template <typename Archive>
        void CEREAL_LOAD_FUNCTION_NAME(Archive& ar)
        {
            std::pair<size_t, std::vector<std::byte>> compressed_table;

            ar(_glyph_bounding_box_size, _glyph_rects, _char_offset, _atlas_size, compressed_table);

            const std::vector<std::byte> decompressed_table =
                decompress_data(compressed_table.second, compressed_table.first);

            std::vector<float> kerning_table;
            kerning_table.reserve(decompressed_table.size() / 4);
            for (size_t i = 0; i < decompressed_table.size(); i += 4)
            {
                kerning_table.push_back(*reinterpret_cast<const float*>(decompressed_table.data() + i));
            }

            _kerning_table = std::move(kerning_table);
        }
    };
} // namespace cathedral::project