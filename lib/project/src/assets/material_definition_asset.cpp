#include <cathedral/project/assets/material_definition_asset.hpp>

#include <cathedral/project/project.hpp>

#include <ien/io_utils.hpp>

#include <magic_enum.hpp>
#include <nlohmann/json.hpp>

namespace cathedral::project
{
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

    void material_definition_asset::save() const
    {
        nlohmann::json json;
        json["asset"] = asset_typestr<SELF>();
        json["material_texture_slots"] = _definition.material_texture_slot_count();
        json["node_texture_slots"] = _definition.node_texture_slot_count();

        nlohmann::json json_material_variables = nlohmann::json::array();
        for (size_t i = 0; i < _definition.material_variables().size(); ++i)
        {
            const auto& var = _definition.material_variables()[i];

            // clang-format off
            json_material_variables[i] = {
                { "type", magic_enum::enum_name(var.type) },
                { "count", var.count },
                { "name", var.name },
                { "binding", var.binding ? magic_enum::enum_name(*var.binding) : "" }
            };
            // clang-format on
        }
        json["material_variables"] = json_material_variables;

        nlohmann::json json_node_variables = nlohmann::json::array();
        for (size_t i = 0; i < _definition.node_variables().size(); ++i)
        {
            const auto& var = _definition.node_variables()[i];

            // clang-format off
            json_node_variables[i] = {
                { "type", magic_enum::enum_name(var.type) },
                { "count", var.count },
                { "name", var.name },
                { "binding", var.binding ? magic_enum::enum_name(*var.binding) : "" }
            };
            // clang-format on
        } // namespace cathedral::project
        json["node_variables"] = json_node_variables;

        write_asset_json(json);
    }

    void material_definition_asset::load()
    {
        const auto& json = get_asset_json();

        CRITICAL_CHECK(json.contains("asset") && json["asset"].get<std::string>() == asset_typestr<SELF>());

        uint32_t material_tex_slots = json["material_texture_slots"].get<uint32_t>();
        uint32_t node_tex_slots = json["node_texture_slots"].get<uint32_t>();

        _definition.set_material_texture_slot_count(material_tex_slots);
        _definition.set_node_texture_slot_count(node_tex_slots);

        const auto var_from_json = [](const nlohmann::json& json_var) -> engine::shader_variable {
            const auto etype = magic_enum::enum_cast<gfx::shader_data_type>(json_var["type"].get<std::string>());
            CRITICAL_CHECK(etype.has_value());

            const auto count = json_var["count"].get<uint32_t>();
            const auto name = json_var["name"].get<std::string>();
            const auto binding =
                magic_enum::enum_cast<engine::shader_uniform_binding>(json_var["binding"].get<std::string>());

            return { *etype, count, name, binding };
        };

        for (const auto& [key, val] : json["material_variables"].items())
        {
            _definition.add_material_variable(var_from_json(val));
        }

        for (const auto& [key, val] : json["node_variables"].items())
        {
            _definition.add_node_variable(var_from_json(val));
        }

        _is_loaded = true;
    }

    void material_definition_asset::unload()
    {
        _definition = {};
        _is_loaded = false;
    }

    std::string material_definition_asset::relative_path() const
    {
        return _path.substr(_project.material_definitions_path().size() + 1);
    }
} // namespace cathedral::project