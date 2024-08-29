#include <cathedral/project/assets/material_asset.hpp>

#include <cathedral/project/json_serializers.hpp>
#include <cathedral/project/project.hpp>

#include <ien/io_utils.hpp>

#include <magic_enum.hpp>
#include <nlohmann/json.hpp>

namespace nlohmann
{
    template <>
    struct adl_serializer<cathedral::project::material_asset_variable_value>
    {
        static void to_json(json& j, const cathedral::project::material_asset_variable_value& value)
        {
            std::visit([&](const auto& v) { j["value"] = v; }, value.value);
            j["type"] = magic_enum::enum_name(value.type);
        }

        static void from_json(const json& j, cathedral::project::material_asset_variable_value& result)
        {
            const auto type = magic_enum::enum_cast<cathedral::gfx::shader_data_type>(j["type"].get<std::string>());
            CRITICAL_CHECK(type.has_value());
            result.type = *type;

            result.value = [&] -> decltype(result.value) {
                switch (*type)
                {
                    using enum cathedral::gfx::shader_data_type;
                case BOOL:
                    return j["value"].get<bool>();
                case INT:
                    return j["value"].get<int32_t>();
                case UINT:
                    return j["value"].get<uint32_t>();
                case FLOAT:
                    return j["value"].get<float>();
                case DOUBLE:
                    return j["value"].get<double>();

                case BVEC2:
                    return j["value"].get<glm::bvec2>();
                case BVEC3:
                    return j["value"].get<glm::bvec3>();
                case BVEC4:
                    return j["value"].get<glm::bvec4>();
                case IVEC2:
                    return j["value"].get<glm::ivec2>();
                case IVEC3:
                    return j["value"].get<glm::ivec3>();
                case IVEC4:
                    return j["value"].get<glm::ivec4>();
                case VEC2:
                    return j["value"].get<glm::vec2>();
                case VEC3:
                    return j["value"].get<glm::vec3>();
                case VEC4:
                    return j["value"].get<glm::vec4>();
                case UVEC2:
                    return j["value"].get<glm::uvec2>();
                case UVEC3:
                    return j["value"].get<glm::uvec3>();
                case UVEC4:
                    return j["value"].get<glm::uvec4>();
                case DVEC2:
                    return j["value"].get<glm::dvec2>();
                case DVEC3:
                    return j["value"].get<glm::dvec3>();
                case DVEC4:
                    return j["value"].get<glm::dvec4>();

                case MAT2X2:
                    return j["value"].get<glm::mat2x2>();
                case MAT2X3:
                    return j["value"].get<glm::mat2x3>();
                case MAT2X4:
                    return j["value"].get<glm::mat2x4>();
                case MAT3X2:
                    return j["value"].get<glm::mat3x2>();
                case MAT3X3:
                    return j["value"].get<glm::mat3x3>();
                case MAT3X4:
                    return j["value"].get<glm::mat3x4>();
                case MAT4X2:
                    return j["value"].get<glm::mat4x2>();
                case MAT4X3:
                    return j["value"].get<glm::mat4x3>();
                case MAT4X4:
                    return j["value"].get<glm::mat4x4>();
                default:
                    CRITICAL_ERROR("Unhandled material asset variable type");
                }
            }();
        }

        static cathedral::project::material_asset_variable_value from_json(const json& j)
        {
            cathedral::project::material_asset_variable_value result;
            from_json(j, result);
            return result;
        }
    };
} // namespace nlohmann

namespace cathedral::project
{
    void material_asset::save() const
    {
        nlohmann::json json;
        json["asset"] = asset_typestr<SELF>();
        json["material_definition_ref"] = _material_definition_ref;
        json["vertex_shader_ref"] = _vertex_shader_ref;
        json["fragment_shader_ref"] = _fragment_shader_ref;

        nlohmann::json texture_slots = nlohmann::json::array();
        for (const auto& slot : _material_texture_slot_refs)
        {
            texture_slots.push_back(slot);
        }
        json["texture_slot_refs"] = texture_slots;

        nlohmann::json variable_values = nlohmann::json::array();
        for (const auto& varval : _material_variable_values)
        {
            std::visit([&](const auto& value) { variable_values.push_back(value); }, varval.value);
        }
        json["variable_values"] = variable_values;

        write_asset_json(json);
    }

    void material_asset::load()
    {
        const auto& json = get_asset_json();

        CRITICAL_CHECK(json.contains("asset") && json["asset"].get<std::string>() == asset_typestr<SELF>());

        CRITICAL_CHECK(json.contains("material_definition_ref"));
        _material_definition_ref = json["material_definition_ref"].get<std::string>();

        CRITICAL_CHECK(json.contains("vertex_shader_ref"));
        _vertex_shader_ref = json["vertex_shader_ref"].get<std::string>();

        CRITICAL_CHECK(json.contains("fragment_shader_ref"));
        _fragment_shader_ref = json["fragment_shader_ref"].get<std::string>();

        _material_texture_slot_refs = {};
        for (const auto& slot : json["texture_slot_refs"])
        {
            _material_texture_slot_refs.push_back(slot.get<std::string>());
        }

        _material_variable_values = {};
        for (const auto& var : json["variable_values"])
        {
            _material_variable_values.push_back(var.get<material_asset_variable_value>());
        }

        _is_loaded = true;
    }

    void material_asset::unload()
    {
        _material_definition_ref = {};
        _material_texture_slot_refs = {};
        _material_variable_values = {};
        _vertex_shader_ref = {};
        _fragment_shader_ref = {};
    }

    std::string material_asset::relative_path() const
    {
        return _path.substr(_project.materials_path().size() + 1);
    }
} // namespace cathedral::project