#pragma once

#include <cathedral/engine/texture.hpp>

#include <cathedral/project/asset.hpp>

namespace cathedral::project
{
    class texture_asset : public asset
    {
    public:
        using asset::asset;

        uint32_t width() const { return _width; }
        void set_width(uint32_t width) { _width = width; }

        uint32_t height() const { return _height; }
        void set_height(uint32_t height) { _height = height; }

        engine::texture_format format() const { return _format; }
        void set_format(engine::texture_format format) { _format = format; }

        const auto& mips() const { return _mips; }
        void set_mips(std::vector<std::vector<uint8_t>> mips) { _mips = std::move(mips); }

        const auto& mip_sizes() const { return _mip_sizes; }
        void set_mip_sizes(std::vector<std::pair<uint32_t, uint32_t>> sizes) { _mip_sizes = sizes; }

        void save() const override;
        void load() override;
        void unload() override;
        std::string relative_path() const override;

    private:
        uint32_t _width;
        uint32_t _height;
        engine::texture_format _format;
        std::vector<std::vector<uint8_t>> _mips;
        std::vector<std::pair<uint32_t, uint32_t>> _mip_sizes;
    };

    template <>
    constexpr std::string asset_typestr<texture_asset>()
    {
        return "texture";
    }
} // namespace cathedral::project