#include <cathedral/project/asset.hpp>

#include <ien/io_utils.hpp>
#include <nlohmann/json.hpp>

#include <filesystem>

namespace cathedral::project
{
    void asset::move_path(const std::string& new_path)
    {
        const auto path = std::filesystem::path(new_path);
        std::filesystem::create_directory(path.parent_path());
        std::filesystem::rename(_path, new_path);
        _path = new_path;
    }

    namespace detail
    {
        bool path_is_asset_typestr(const std::string& path, const std::string& typestr)
        {
            const auto text = ien::read_file_text(path);
            CRITICAL_CHECK(text.has_value());
            const auto json = nlohmann::json::parse(*text);
            return json.contains("asset") && json["asset"] == typestr;
        }
    } // namespace detail

    nlohmann::json asset::get_asset_json() const
    {
        const auto text = ien::read_file_text(_path);
        CRITICAL_CHECK(text.has_value());
        return nlohmann::json::parse(*text);
    }

    void asset::write_asset_json(const nlohmann::json& j) const
    {
        std::filesystem::create_directories(std::filesystem::path(_path).parent_path());
        bool write_ok = ien::write_file_text(_path, j.dump(2));
        CRITICAL_CHECK(write_ok);
    }
} // namespace cathedral::project