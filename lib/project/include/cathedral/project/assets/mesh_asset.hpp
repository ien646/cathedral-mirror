#pragma once

#include <cathedral/engine/mesh.hpp>

#include <cathedral/project/asset.hpp>

#include <cereal/types/base_class.hpp>

namespace cathedral::project
{
    class mesh_asset final : public asset
    {
    public:
        using asset::asset;

        CATHEDRAL_ASSET_SUBCLASS_DECL

        uint32_t uncompressed_size() const { return _uncompressed_data_size; }

        void save_mesh(const engine::mesh& mesh);
        [[nodiscard]] engine::mesh load_mesh() const;

        constexpr const char* typestr() const override { return "mesh"; };

    private:
        uint32_t _uncompressed_data_size = 0;

        friend class cereal::access;

        template <typename Archive>
        void CEREAL_SERIALIZE_FUNCTION_NAME(Archive& ar)
        {
            ar(cereal::make_nvp("asset", cereal::base_class<asset>(this)),
               cereal::make_nvp("uncompressed_data_size", _uncompressed_data_size));
        }
    };

} // namespace cathedral::project