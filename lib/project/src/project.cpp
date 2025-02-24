#include <cathedral/project/project.hpp>

#include <cathedral/project/serialization/scene.hpp>

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

        auto lines = ien::str_splitv(*text, '\n') |
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

        if (kvs.contains("project-name"))
        {
            _project_name = kvs["project_name"];
        }

        _root_path = project_path;
        _shaders_path = (std::filesystem::path(project_path) / "shaders").string();
        _material_definitions_path = (std::filesystem::path(project_path) / "material_definitions").string();
        _materials_path = (std::filesystem::path(project_path) / "materials").string();
        _textures_path = (std::filesystem::path(project_path) / "textures").string();
        _meshes_path = (std::filesystem::path(project_path) / "meshes").string();

        load_shader_assets();
        load_material_definition_assets();
        load_texture_assets();
        load_material_assets();
        load_mesh_assets();

        return load_project_status::OK;
    }

    void project::reload_shader_assets()
    {
        load_shader_assets();
    }

    void project::reload_material_definition_assets()
    {
        load_material_definition_assets();
    }

    void project::reload_texture_assets()
    {
        load_texture_assets();
    }

    void project::reload_material_assets()
    {
        load_material_assets();
    }

    void project::reload_mesh_assets()
    {
        load_mesh_assets();
    }

    void project::save_scene(const engine::scene& scene) const
    {
        std::stringstream sstr;
        {
            cereal::JSONOutputArchive archive(sstr);
            archive(scene);
        }
        ien::write_file_text("/tmp/scene.casset", sstr.str());
    }

    project project::create(const std::string& path, const std::string& name)
    {
        const auto project_path = std::filesystem::path(path);
        const auto project_file_path = project_path / ".cathedral";
        ien::write_file_text(project_file_path.string(), std::format("project-name:{}", name));

        std::filesystem::create_directories(project_path / "material_definitions");
        std::filesystem::create_directories(project_path / "materials");
        std::filesystem::create_directories(project_path / "meshes");
        std::filesystem::create_directories(project_path / "shaders");
        std::filesystem::create_directories(project_path / "textures");

        project result;
        result.load_project(path);

        return result;
    }

    template <AssetLike TAsset>
    void project::load_assets(
        const std::string& path,
        std::unordered_map<std::string, std::shared_ptr<TAsset>>& target_container)
    {
        target_container.clear();

        if (!std::filesystem::exists(path))
        {
            std::filesystem::create_directories(path);
        }

        for (const auto& f : std::filesystem::recursive_directory_iterator(path))
        {
            if (f.is_regular_file() && f.path().extension() == ".casset")
            {
                const auto strpath = f.path().string();
                auto ast = std::make_shared<TAsset>(this, strpath);
                ast->load();
                target_container.emplace(ast->absolute_path(), ast);
            }
        }
    }

    void project::load_shader_assets()
    {
        load_assets(_shaders_path, _shader_assets);
    }

    void project::load_material_definition_assets()
    {
        load_assets(_material_definitions_path, _material_definition_assets);
    }

    void project::load_texture_assets()
    {
        load_assets(_textures_path, _texture_assets);
    }

    void project::load_material_assets()
    {
        load_assets(_materials_path, _material_assets);
    }

    void project::load_mesh_assets()
    {
        load_assets(_meshes_path, _mesh_assets);
    }
} // namespace cathedral::project