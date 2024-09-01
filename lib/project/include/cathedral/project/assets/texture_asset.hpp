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

        const auto& mip_sizes() const { return _mip_sizes; }

        void set_mip_sizes(const std::vector<std::pair<uint32_t, uint32_t>>& sizes) { _mip_sizes = sizes; }

        void save() const override;
        void load() override;
        void unload() override;
        std::string relative_path() const override;

        [[nodiscard]] std::vector<std::vector<std::byte>> load_mips() const;
        [[nodiscard]] std::vector<std::byte> load_single_mip(uint32_t mip_index) const;
        void save_mips(const std::vector<std::vector<std::byte>>& mips) const;

        size_t texture_size_bytes() const;

        static uint32_t get_closest_sized_mip_index(
            uint32_t width,
            uint32_t height,
            const std::vector<std::pair<uint32_t, uint32_t>>& mip_sizes);

    private:
        uint32_t _width;
        uint32_t _height;
        engine::texture_format _format;
        std::vector<std::pair<uint32_t, uint32_t>> _mip_sizes;
    };

    template <>
    constexpr std::string asset_typestr<texture_asset>()
    {
        return "texture";
    }
} // namespace cathedral::project