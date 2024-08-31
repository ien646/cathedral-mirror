#include <cathedral/project/assets/material_definition_asset.hpp>

#include <ien/io_utils.hpp>

#include <magic_enum.hpp>
#include <nlohmann/json.hpp>

namespace cathedral::project
{
    void material_definition_asset::set_definition(engine::material_definition def)
    {
        _definition = std::move(def);
    }

    const engine::material_definition& material_definition_asset::get_definition() const
    {
        return _definition;
    }

    void material_definition_asset::save() const
    {
        nlohmann::json json;
        json["asset"] = asset_typestr<material_definition_asset>();
        json["material_texture_slots"] = _definition.material_texture_slot_count();
        json["node_texture_slots"] = _definition.node_texture_slot_count();

        nlohmann::json json_material_variables = nlohmann::json::array();
        for (const auto& [index, var] : _definition.material_variables())
        {
            // clang-format off
            json_material_variables[index] = {
                {"type", magic_enum::enum_name(var.type)},
                {"count", var.count},
                {"name", var.name}
            };
            // clang-format on
        }
        json["material_variables"] = json_material_variables;

        nlohmann::json json_node_variables = nlohmann::json::array();
        for (const auto& [index, var] : _definition.node_variables())
        {
            // clang-format off
            json_node_variables[index] = {
                {"type", magic_enum::enum_name(var.type)},
                {"count", var.count},
                {"name", var.name}
            };
            // clang-format on
        }
        json["node_variables"] = json_node_variables;

        std::filesystem::create_directories(std::filesystem::path(_path).parent_path());

        bool write_ok = ien::write_file_text(_path, json.dump(2));
        CRITICAL_CHECK(write_ok);
    }

    void material_definition_asset::load()
    {
        const auto text = ien::read_file_text(_path);
        CRITICAL_CHECK(text.has_value());

        auto json = nlohmann::json::parse(*text);
        CRITICAL_CHECK(
            json.contains("asset") && json["asset"].get<std::string>() == asset_typestr<material_definition_asset>());

        uint32_t material_tex_slots = json["material_texture_slots"].get<uint32_t>();
        uint32_t node_tex_slots = json["node_texture_slots"].get<uint32_t>();

        _definition.set_material_texture_slot_count(material_tex_slots);
        _definition.set_node_texture_slot_count(node_tex_slots);

        const auto var_from_json = [](const nlohmann::json& json_var) -> engine::material_definition::variable {
            const auto etype = magic_enum::enum_cast<gfx::shader_data_type>(json_var["type"].get<std::string>());
            CRITICAL_CHECK(etype.has_value());

            const auto count = json_var["count"].get<uint32_t>();
            const auto name = json_var["name"].get<std::string>();
            const auto binding =
                magic_enum::enum_cast<engine::material_uniform_binding>(json_var["binding"].get<std::string>());

            return { *etype, count, name, binding };
        };

        for (const auto& [index, json_var] : json["material_variables"].items())
        {
            _definition.set_material_variable(std::stoul(index), var_from_json(json_var));
        }

        for (const auto& [index, json_var] : json["node_variables"].items())
        {
            _definition.set_node_variable(std::stoul(index), var_from_json(json_var));
        }

        _is_loaded = true;
    }

    void material_definition_asset::unload()
    {
        _definition = { };
    }
} // namespace cathedral::project