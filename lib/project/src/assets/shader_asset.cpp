#include <cathedral/project/assets/shader_asset.hpp>

#include <cathedral/project/project.hpp>

#include <ien/io_utils.hpp>

#include <magic_enum.hpp>
#include <nlohmann/json.hpp>

namespace cathedral::project
{
    void shader_asset::load()
    {
        const auto& json = get_asset_json();
        
        CRITICAL_CHECK(json.contains("asset") && json["asset"].get<std::string>() == asset_typestr<shader_asset>());
        const auto shader_type = magic_enum::enum_cast<gfx::shader_type>(json["type"].get<std::string>());
        CRITICAL_CHECK(shader_type.has_value());
        _type = *shader_type;
        _source = json["source"].get<std::string>();

        _is_loaded = true;
    }

    void shader_asset::save() const
    {
        nlohmann::json json;
        json["asset"] = asset_typestr<shader_asset>();
        json["type"] = magic_enum::enum_name(_type);
        json["source"] = _source;

        std::filesystem::create_directories(std::filesystem::path(_path).parent_path());

        bool write_ok = ien::write_file_text(_path, json.dump(2));
        CRITICAL_CHECK(write_ok);
    }

    void shader_asset::unload()
    {
        _type = gfx::shader_type::UNDEFINED;
        _source = {};

        _is_loaded = false;
    }

    std::string shader_asset::relative_path() const
    {
        return _path.substr(_project.shaders_path().size());
    }
}