#include <cathedral/project/project.hpp>

#include <ien/fs_utils.hpp>
#include <ien/io_utils.hpp>
#include <ien/str_utils.hpp>

#include <ranges>
#include <unordered_map>

namespace cathedral::project
{
    load_project_status project::load_project(const std::string& project_path)
    {
        if (!ien::directory_exists(project_path))
        {
            return load_project_status::PROJECT_PATH_NOT_FOUND;
        }

        const auto project_file_path = std::filesystem::path(project_path) / ".cathedral";
        if (!std::filesystem::exists(project_file_path))
        {
            return load_project_status::PROJECT_FILE_NOT_FOUND;
        }

        const auto text = ien::read_file_text(project_file_path.string());
        if (!text)
        {
            return load_project_status::PROJECT_FILE_READ_FAILURE;
        }

        auto lines =
            ien::str_splitv(*text, '\n') |
            std::views::filter([](std::string_view str) { return !ien::str_trim(str).empty(); });

        std::unordered_map<std::string, std::string> kvs;
        for (const auto& ln : lines)
        {
            const auto segments = ien::str_splitv(ln, ':');
            if (segments.size() < 2)
            {
                continue;
            }

            kvs.emplace(std::string{ ien::str_trim(segments[0]) }, ien::str_trim(ln.substr(segments[0].size() + 1)));
        }

        if (kvs.count("project-name"))
        {
            _project_name = kvs["project_name"];
        }

        _root_path = project_path;
        _shaders_path = (std::filesystem::path(project_path) / "shaders").string();
        _material_definitions_path = (std::filesystem::path(project_path) / "material_definitions").string();
        _materials_path = (std::filesystem::path(project_path) / "materials").string();
        _textures_path = (std::filesystem::path(project_path) / "textures").string();

        load_shader_assets();
        load_material_definition_assets();

        return load_project_status::OK;
    }

    void project::add_asset(std::shared_ptr<shader_asset> asset)
    {
        _shader_assets.emplace(asset->path(), asset);
    }

    void project::add_asset(std::shared_ptr<material_definition_asset> asset)
    {
        _material_definition_assets.emplace(asset->path(), asset);
    }

    void project::reload_shader_assets()
    {
        load_shader_assets();
    }    

    void project::reload_material_definition_assets()
    {
        load_material_definition_assets();
    }

    template <AssetLike TAsset, typename TContainer>
    void project::load_assets(const std::string& path, TContainer& target_container)
    {
        for (const auto& f : std::filesystem::recursive_directory_iterator(path))
        {
            if (f.is_regular_file() && f.path().extension() == ".casset")
            {
                const auto strpath = f.path().string();
                if (!path_is_asset_type<TAsset>(strpath))
                {
                    continue;
                }
                auto ast = std::make_shared<TAsset>(*this, strpath);
                target_container.emplace(ast->path(), ast);
            }
        }
    }

    void project::load_shader_assets()
    {
        if(!std::filesystem::exists(_shaders_path))
        {
            std::filesystem::create_directories(_shaders_path);
        }

        _shader_assets.clear();
        load_assets<shader_asset>(_shaders_path, _shader_assets);
    }

    void project::load_material_definition_assets()
    {
        if(!std::filesystem::exists(_material_definitions_path))
        {
            std::filesystem::create_directories(_material_definitions_path);
        }
        _material_definition_assets.clear();
        load_assets<material_definition_asset>(_material_definitions_path, _material_definition_assets);
    }
} // namespace cathedral::project