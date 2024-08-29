#include <cathedral/project/asset.hpp>

#include <filesystem>

namespace cathedral::project
{
    void asset::move_path(const std::string& new_path)
    {
        std::filesystem::rename(_path, new_path);
        _path = new_path;
    }
} // namespace cathedral::project