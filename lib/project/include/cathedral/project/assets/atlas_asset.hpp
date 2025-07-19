#pragma once

#include <cathedral/engine/atlas.hpp>
#include <cathedral/project/asset.hpp>
#include <cathedral/project/serialization/atlas.hpp>

namespace cathedral::project
{
    class atlas_asset : public asset
    {
    public:
        using asset::asset;

        CATHEDRAL_ASSET_SUBCLASS_DECL

        void set_atlas(engine::atlas atlas);
        const engine::atlas& atlas() const;

    private:
        engine::atlas _atlas;

        friend class cereal::access;

        template <typename Archive>
        void CEREAL_SERIALIZE_FUNCTION_NAME(Archive& ar)
        {
            ar(cereal::make_nvp("asset", cereal::base_class<asset>(this)), cereal::make_nvp("atlas", _atlas));
        }
    };
} // namespace cathedral::project