#include <cathedral/project/assets/dynamic_script_asset.hpp>

#include <cathedral/project/project.hpp>

#include <ien/io_utils.hpp>

namespace cathedral::project
{
    CATHEDRAL_ASSET_SUBCLASS_IMPL(dynamic_script_asset);

    constexpr auto IMPORT_EXTENSION = ".lua";

    std::string dynamic_script_asset::import_path() const
    {
        constexpr auto EXTENSION_SIZE = std::string{ ".casset" }.size();
        return _path.substr(0, _path.size() - EXTENSION_SIZE) + IMPORT_EXTENSION;
    }

    bool dynamic_script_asset::can_import() const
    {
        return std::filesystem::exists(import_path());
    }

    void dynamic_script_asset::import()
    {
        const std::optional<std::string> content = ien::read_file_text(import_path());
        CRITICAL_CHECK(content.has_value(), "Failed to read shader asset import file");
        set_source(*content);
        save();
    }
}