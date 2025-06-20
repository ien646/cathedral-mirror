#include <cathedral/project/assets/shader_asset.hpp>

#include <cathedral/project/project.hpp>

#include <ien/fs_utils.hpp>
#include <ien/io_utils.hpp>

#include <magic_enum.hpp>

namespace cathedral::project
{
    CATHEDRAL_ASSET_SUBCLASS_IMPL(shader_asset);

    constexpr auto IMPORT_EXTENSION = ".glsl";

    std::string shader_asset::import_path() const
    {
        constexpr auto EXTENSION_SIZE = std::string{ ".casset" }.size();
        return _path.substr(0, _path.size() - EXTENSION_SIZE) + IMPORT_EXTENSION;
    }

    bool shader_asset::can_import() const
    {
        return std::filesystem::exists(import_path());
    }

    void shader_asset::import()
    {
        const std::optional<std::string> content = ien::read_file_text(import_path());
        CRITICAL_CHECK(content.has_value(), "Failed to read shader asset import file");
        set_source(*content);
        save();
    }
} // namespace cathedral::project