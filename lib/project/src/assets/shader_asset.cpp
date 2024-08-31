#include <cathedral/project/assets/shader_asset.hpp>

#include <ien/io_utils.hpp>

#include <nlohmann/json.hpp>

namespace cathedral::project
{
    bool shader_asset::is_loaded() const
    {
        return _type != gfx::shader_type::UNDEFINED;
    }

    void shader_asset::load()
    {
        const auto text = ien::read_file_text(_path);
        CRITICAL_CHECK(text.has_value());

        auto json = nlohmann::json::parse(*text);
        _type = static_cast<gfx::shader_type>(json["type"].get<uint32_t>());
        _source = json["source"].get<std::string>();
        _spirv = json["spirv"].get<std::vector<uint32_t>>();
    }

    void shader_asset::save() const
    {
        nlohmann::json json;
        json["type"] = static_cast<uint32_t>(_type);
        json["source"] = _source;
        json["spirv"] = _spirv;

        bool write_ok = ien::write_file_text(_path, json.dump(2));
        CRITICAL_CHECK(write_ok);
    }

    void shader_asset::unload()
    {
        _type = gfx::shader_type::UNDEFINED;
        _source = {};
        _spirv = {};
    }
}