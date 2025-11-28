#include <cathedral/project/asset.hpp>

#include <cathedral/project/project.hpp>

#include <ien/io_utils.hpp>

#include <filesystem>

namespace cathedral::project
{
    void asset::move_path(const std::string& new_path)
    {
        const auto path = std::filesystem::path(new_path);
        const auto old_binpath = bin_path();

        std::filesystem::create_directory(path.parent_path());
        std::filesystem::rename(_path, new_path);
        _path = new_path;

        if (std::filesystem::exists(old_binpath))
        {
            std::filesystem::rename(old_binpath, bin_path());
        }
    }

    void asset::set_path_by_relpath(const std::string& relpath)
    {
        const auto& assets_path = _project->get_assets_path_by_typestr(typestr());
        _path = assets_path + '/' + relpath;
    }

    void asset::write_asset_binary(const std::vector<std::byte>& data) const
    {
        const auto path = bin_path();
        std::filesystem::create_directories(std::filesystem::path(path).parent_path());

        [[maybe_unused]] const bool write_ok = ien::write_file_binary(path, data);
        CRITICAL_CHECK(write_ok, "Failure writing asset binary file");
    }

    std::string asset::bin_path() const
    {
        std::filesystem::path fspath(_path);
        fspath.replace_extension(".lz4");
        return fspath.string();
    }

    std::string asset::thumbnail_path() const
    {
        std::filesystem::path fspath(_path);
        fspath.replace_extension(".png");
        return fspath.string();
    }

    std::optional<ien::image> asset::load_thumbnail() const
    {
        if (!std::filesystem::exists(thumbnail_path()))
        {
            return {};
        }
        return ien::image(thumbnail_path());
    }

    void asset::set_thumbnail(const ien::image& img) const
    {
        if (img.width() != 64 || img.height() != 64)
        {
            const auto resized_image = img.resize(64, 64);
            resized_image.write_to_file_png(thumbnail_path());
        }
        else
        {
            img.write_to_file_png(thumbnail_path());
        }
    }

    bool asset::has_thumbnail() const
    {
        return std::filesystem::exists(thumbnail_path());
    }
} // namespace cathedral::project