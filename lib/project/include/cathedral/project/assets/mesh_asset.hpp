#pragma once

#include <cathedral/engine/mesh.hpp>

#include <cathedral/project/asset.hpp>

namespace cathedral::project
{
    class mesh_asset : public asset
    {
    public:
        using asset::asset;

        uint32_t uncompressed_size() const { return _uncompressed_data_size; }

        void save() const override;
        void load() override;
        void unload() override;

        std::string relative_path() const override;

        void save_mesh(const engine::mesh& mesh);
        [[nodiscard]] engine::mesh load_mesh() const;

    private:
        uint32_t _uncompressed_data_size = 0;
    };

    template <>
    constexpr std::string asset_typestr<mesh_asset>()
    {
        return "mesh";
    }
} // namespace cathedral::project