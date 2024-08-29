#include <cathedral/project/assets/texture_asset.hpp>

#include <cathedral/project/project.hpp>

#include <ien/io_utils.hpp>

#include <magic_enum.hpp>
#include <nlohmann/json.hpp>

namespace cathedral::project
{
    void texture_asset::save() const
    {
        nlohmann::json json;
        json["asset"] = asset_typestr<texture_asset>();
        json["width"] = _width;
        json["height"] = _height;
        json["format"] = magic_enum::enum_name(_format);

        nlohmann::json mips = nlohmann::json::array();
        for (size_t i = 0; i < _mips.size(); ++i)
        {
            mips[i] = _mips[i];
        }
        json["mips"] = mips;

        bool write_ok = ien::write_file_text(_path, json.dump(2));
        CRITICAL_CHECK(write_ok);
    }

    void texture_asset::load()
    {
        const auto& json = get_asset_json();
        CRITICAL_CHECK(json.contains("asset") && json["asset"].get<std::string>() == asset_typestr<texture_asset>());

        _width = json["width"].get<uint32_t>();
        _height = json["height"].get<uint32_t>();
        const auto format = magic_enum::enum_cast<engine::texture_format>(json["width"].get<std::string>());
        CRITICAL_CHECK(format.has_value());
        _format = *format;

        _mips.clear();
        for (const auto& [key, val] : json["mips"].items())
        {
            _mips.push_back(val.get<std::vector<uint8_t>>());
        }

        _is_loaded = true;
    }

    void texture_asset::unload()
    {
        _width = 0;
        _height = 0;
        _mips.clear();
        _is_loaded = false;
    }

    std::string texture_asset::relative_path() const
    {
        return _path.substr(_project.textures_path().size() + 1);
    }
} // namespace cathedral::project