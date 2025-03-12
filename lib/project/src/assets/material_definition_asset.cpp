#include <cathedral/project/assets/material_definition_asset.hpp>

#include <cathedral/project/project.hpp>

#include <ien/io_utils.hpp>

#include <nlohmann/json.hpp>

#include <fstream>

namespace cathedral::project
{
    CATHEDRAL_ASSET_SUBCLASS_IMPL(material_definition_asset);

    void material_definition_asset::set_definition(engine::material_definition def)
    {
        _definition = std::move(def);
    }

    engine::material_definition& material_definition_asset::get_definition()
    {
        return _definition;
    }

    const engine::material_definition& material_definition_asset::get_definition() const
    {
        return _definition;
    }
} // namespace cathedral::project