#include <cathedral/project/asset.hpp>

#include <ien/io_utils.hpp>
#include <nlohmann/json.hpp>

#include <filesystem>

namespace cathedral::project
{
    void asset::move_path(const std::string& new_path)
    {
        std::filesystem::rename(_path, new_path);
        _path = new_path;
    }

    namespace detail
    {
        bool path_is_asset_typestr(const std::string& path, const std::string& typestr)
        {
            const auto text = ien::read_file_text(path);
            const auto json = nlohmann::json::parse(path);
            return json.contains("asset") && json["asset"] == typestr;
        }
    } // namespace detail
} // namespace cathedral::project