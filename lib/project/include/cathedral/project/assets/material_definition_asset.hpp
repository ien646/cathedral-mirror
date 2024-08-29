#pragma once

#include <cathedral/engine/material_definition.hpp>

#include <cathedral/project/asset.hpp>

namespace cathedral::project
{
    class material_definition_asset : public asset
    {
    public:
        using asset::asset;

        void set_definition(engine::material_definition);
        const engine::material_definition& get_definition() const;

        void save() const override;
        void load() override;
        void unload() override;

    private:
        engine::material_definition _definition;
    };

    template <>
    constexpr std::string asset_typestr<material_definition_asset>()
    {
        return "material-definition";
    }
} // namespace cathedral::project