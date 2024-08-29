#include <cathedral/project/assets/shader_asset.hpp>

#include <ien/io_utils.hpp>

#include <nlohmann/json.hpp>

namespace cathedral::project
{
    void shader_asset::load()
    {
        const auto text = ien::read_file_text(_path);
        CRITICAL_CHECK(text.has_value());

        auto json = nlohmann::json::parse(*text);
        CRITICAL_CHECK(json.contains("asset") && json["asset"].get<std::string>() == asset_typestr<shader_asset>());
        _type = static_cast<gfx::shader_type>(json["type"].get<uint32_t>());
        _source = json["source"].get<std::string>();

        _is_loaded = true;
    }

    void shader_asset::save() const
    {
        nlohmann::json json;
        json["asset"] = asset_typestr<shader_asset>();
        json["type"] = static_cast<uint32_t>(_type);
        json["source"] = _source;

        bool write_ok = ien::write_file_text(_path, json.dump(2));
        CRITICAL_CHECK(write_ok);
    }

    void shader_asset::unload()
    {
        _type = gfx::shader_type::UNDEFINED;
        _source = {};

        _is_loaded = false;
    }
}