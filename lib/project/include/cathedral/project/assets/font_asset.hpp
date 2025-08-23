#pragma once

#include "cathedral/compression.hpp"

#include <cathedral/engine/font.hpp>
#include <cathedral/project/asset.hpp>

#include <cathedral/glm_serializers.hpp>

#include <cereal/cereal.hpp>
#include <cereal/types/vector.hpp>
#include <cereal/types/utility.hpp>

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

        void set_kerning_table(std::vector<std::vector<float>> table);
        std::vector<std::vector<float>> kerning_table() const;

        constexpr const char* typestr() const override { return "font"; }

    private:
        glm::ivec2 _glyph_bounding_box_size;
        std::vector<engine::font_glyph_info> _glyph_rects;
        uint32_t _char_offset = 0;
        glm::uvec2 _atlas_size;
        std::vector<std::vector<float>> _kerning_table;

        friend class cereal::access;

        template <typename Archive>
        void CEREAL_SAVE_FUNCTION_NAME(Archive& ar) const
        {
            std::vector<std::vector<std::byte>> kerning_table_bytes;
            kerning_table_bytes.reserve(_kerning_table.size());

            for (const auto& row : _kerning_table)
            {
                kerning_table_bytes.emplace_back();
                kerning_table_bytes.back().reserve(row.size() * sizeof(float));
                for (const auto& byte : std::as_bytes(std::span{row.data(), row.size()}))
                {
                    kerning_table_bytes.back().push_back(byte);
                }
            }

            std::vector<std::pair<size_t, std::vector<std::byte>>> compressed_rows;
            compressed_rows.reserve(kerning_table_bytes.size());
            for (auto& row_bytes : kerning_table_bytes)
            {
                compressed_rows.emplace_back(row_bytes.size(), compress_data(row_bytes));
            }

            ar(cereal::make_nvp("glyph_bounding_box", _glyph_bounding_box_size),
               cereal::make_nvp("glyph_rects", _glyph_rects),
               cereal::make_nvp("char_offset", _char_offset),
               cereal::make_nvp("atlas_size", _atlas_size),
               cereal::make_nvp("kerning_table", compressed_rows));
        }

        template <typename Archive>
        void CEREAL_LOAD_FUNCTION_NAME(Archive& ar)
        {
            std::vector<std::pair<size_t, std::vector<std::byte>>> compressed_rows;

            ar(_glyph_bounding_box_size, _glyph_rects, _char_offset, _atlas_size, compressed_rows);

            std::vector<std::vector<float>> kerning_table;
            kerning_table.reserve(compressed_rows.size());

            for (const auto& [uncompressed_size, data] : compressed_rows)
            {
                kerning_table.emplace_back();

                std::vector<std::byte> decompressed_row = decompress_data(data, uncompressed_size);
                CRITICAL_CHECK(decompressed_row.size() % 4 == 0, "Invalid decompressed kerning table data");
                for (size_t i = 0; i < decompressed_row.size(); i += 4)
                {
                    kerning_table.back().push_back(*reinterpret_cast<float*>(decompressed_row.data() + i));
                }
            }

            _kerning_table = std::move(kerning_table);
        }
    };
} // namespace cathedral::project