#include <cathedral/project/assets/material_asset.hpp>

#include <cathedral/project/project.hpp>

#include <ien/io_utils.hpp>
#include <nlohmann/json.hpp>

namespace cathedral::project
{
    void material_asset::load()
    {
        const auto text = ien::read_file_text(_path);
        CRITICAL_CHECK(text.has_value());

        auto json = nlohmann::json::parse(*text);
        CRITICAL_CHECK(json.contains("asset") && json["asset"].get<std::string>() == asset_typestr<material_asset>());
        CRITICAL_CHECK(json.contains("vertex_shader"));
        CRITICAL_CHECK(json.contains("fragment_shader"));

        const auto vertex_shader_path = json["vertex_shader"].get<std::string>();
        const auto fragment_shader_path = json["fragment_shader"].get<std::string>();

        _material_domain = static_cast<engine::material_domain>(json["type"].get<uint32_t>());
        _vertex_shader =
            vertex_shader_path.empty() ? decltype(_vertex_shader){} : _project.shader_assets().at(vertex_shader_path);
        _fragment_shader =
            fragment_shader_path.empty() ? decltype(_fragment_shader){} : _project.shader_assets().at(fragment_shader_path);

        _is_loaded = true;
    }

    void material_asset::save() const
    {
        nlohmann::json json;
        json["asset"] = asset_typestr<material_asset>();
        json["material_domain"] = static_cast<uint32_t>(_material_domain);
        json["vertex_shader"] = _vertex_shader ? "" : _vertex_shader->path();
        json["fragment_shader"] = _fragment_shader ? "" : _fragment_shader->path();

        bool write_ok = ien::write_file_text(_path, json.dump(2));
        CRITICAL_CHECK(write_ok);
    }

    void material_asset::unload()
    {
        _material_domain = {};
        _vertex_shader = {};
        _fragment_shader = {};

        _is_loaded = false;
    }
} // namespace cathedral::project