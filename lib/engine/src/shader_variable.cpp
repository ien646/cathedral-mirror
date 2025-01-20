#include <cathedral/engine/shader_variable.hpp>

#include <magic_enum.hpp>
#include <nlohmann/json.hpp>

namespace cathedral::engine
{
    nlohmann::json shader_variable::to_json() const
    {
        nlohmann::json json;
        json["type"] = magic_enum::enum_name(type);
        json["count"] = count;
        json["name"] = name;
        json["binding"] = binding ? "" : magic_enum::enum_name(*binding);
        return json;
    }

    void shader_variable::from_json(const nlohmann::json& json)
    {
        const auto type_opt = magic_enum::enum_cast<decltype(type)>(json["type"].get<std::string>());
        CRITICAL_CHECK(type_opt.has_value());
        type = *type_opt;

        json["count"].get_to(count);
        json["name"].get_to(name);

        binding = json["binding"].empty()
                      ? std::nullopt
                      : magic_enum::enum_cast<shader_uniform_binding>(json["binding"].get<std::string>());
    }
} // namespace cathedral::engine