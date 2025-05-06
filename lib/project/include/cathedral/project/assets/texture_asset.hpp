#pragma once

#include <cathedral/engine/texture.hpp>

#include <cathedral/project/asset.hpp>
#include <cathedral/project/serialization/enums.hpp>
#include <cathedral/project/serialization/sampler_info.hpp>

#include <cathedral/glm_serializers.hpp>

#include <cereal/types/base_class.hpp>
#include <cereal/types/polymorphic.hpp>
#include <cereal/types/string.hpp>
#include <cereal/types/vector.hpp>

namespace cathedral::project
{
    class texture_asset final : public asset
    {
    public:
        using asset::asset;

        CATHEDRAL_ASSET_SUBCLASS_DECL

        uint32_t width() const { return _width; }

        auto set_width(const uint32_t width) -> void { _width = width; }

        uint32_t height() const { return _height; }

        void set_height(const uint32_t height) { _height = height; }

        engine::texture_format format() const { return _format; }

        void set_format(const engine::texture_format format) { _format = format; }

        const auto& mip_sizes() const { return _mip_dimensions; }

        void set_mip_dimensions(const std::vector<glm::uvec2>& sizes) { _mip_dimensions = sizes; }

        [[nodiscard]] std::vector<std::vector<std::byte>> load_mips() const;
        [[nodiscard]] std::vector<std::byte> load_single_mip(uint32_t mip_index) const;
        void save_mips(const std::vector<std::vector<std::byte>>& mips) const;

        const auto& sampler_info() const { return _sampler_info; }

        size_t texture_size_bytes() const;

        static uint32_t get_closest_sized_mip_index(uint32_t width, uint32_t height, const std::vector<glm::uvec2>& mip_sizes);

        constexpr const char* typestr() const override { return "texture"; };

    private:
        uint32_t _width = 0;
        uint32_t _height = 0;
        engine::texture_format _format = engine::texture_format::R8G8B8A8_LINEAR;
        gfx::sampler_info _sampler_info;
        std::vector<glm::uvec2> _mip_dimensions;

        friend class cereal::access;

        template <typename Archive>
        void CEREAL_SERIALIZE_FUNCTION_NAME(Archive& ar)
        {
            ar(cereal::make_nvp("asset", cereal::base_class<asset>(this)),
               cereal::make_nvp("width", _width),
               cereal::make_nvp("height", _height),
               cereal::make_nvp("format", _format),
               cereal::make_nvp("mip_sizes", _mip_dimensions),
               cereal::make_nvp("sampler_info", _sampler_info));
        }
    };
} // namespace cathedral::project