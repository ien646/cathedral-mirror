#pragma once

#include <cathedral/engine/material_definition.hpp>

#include <cathedral/project/asset.hpp>

#include <cathedral/project/serialization/material_definition.hpp>

#include <cereal/access.hpp>
#include <cereal/types/base_class.hpp>

namespace cathedral::project
{
    class material_definition_asset : public asset
    {
    public:
        using asset::asset;

        CATHEDRAL_ASSET_SUBCLASS_DECL

        void set_definition(engine::material_definition);

        engine::material_definition& get_definition();
        const engine::material_definition& get_definition() const;

        constexpr const char* typestr() const override { return "material-definition"; };

    private:
        engine::material_definition _definition;

        friend class cereal::access;

        template <typename Archive>
        void CEREAL_SERIALIZE_FUNCTION_NAME(Archive& ar)
        {
            ar(cereal::make_nvp("asset", cereal::base_class<asset>(this)), cereal::make_nvp("definition", _definition));
        }
    };
} // namespace cathedral::project