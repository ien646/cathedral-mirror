#include <cathedral/project/project.hpp>

#include <cathedral/project/serialization/scene.hpp>

#include <ien/fs_utils.hpp>
#include <ien/io_utils.hpp>
#include <ien/str_utils.hpp>

#include <cereal/archives/json.hpp>

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
        _materials_path = (std::filesystem::path(project_path) / "materials").string();
        _textures_path = (std::filesystem::path(project_path) / "textures").string();
        _meshes_path = (std::filesystem::path(project_path) / "meshes").string();
        _scenes_path = (std::filesystem::path(project_path) / "scenes").string();

        load_shader_assets();
        load_texture_assets();
        load_material_assets();
        load_mesh_assets();

        return load_project_status::OK;
    }

    void project::reload_shader_assets()
    {
        load_shader_assets();
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

    engine::scene_loader_funcs project::get_loader_funcs() const
    {
        engine::scene_loader_funcs result;

        result.material_loader = [this](const std::string& name, engine::scene& scene) -> std::weak_ptr<engine::material> {
            auto asset = _material_assets.at(name);
            auto& renderer = scene.get_renderer();
            if (renderer.materials().contains(asset->name()))
            {
                return renderer.materials().at(asset->name());
            }

            const auto vertex_shader_name = asset->vertex_shader_ref();
            const auto fragment_shader_name = asset->fragment_shader_ref();

            if (vertex_shader_name.empty() || fragment_shader_name.empty())
            {
                return {};
            }

            engine::material_args args;
            args.name = asset->name();
            args.vertex_shader = scene.load_shader(vertex_shader_name);
            args.fragment_shader = scene.load_shader(fragment_shader_name);
            args.domain = asset->domain();

            auto result = renderer.create_material(args).lock();
            for (size_t i = 0; i < asset->texture_slot_refs().size(); ++i)
            {
                const auto& name = asset->texture_slot_refs()[i];
                auto texture = scene.load_texture(name);
                result->bind_material_texture_slot(texture, i);
            }

            return result;
        };

        result.mesh_loader =
            [this](const std::string& name, [[maybe_unused]] const engine::scene& scene) -> std::shared_ptr<engine::mesh> {
            auto asset = _mesh_assets.at(name);
            return std::make_shared<engine::mesh>(asset->load_mesh());
        };

        result.shader_loader = [this](const std::string& name, engine::scene& scene) -> std::shared_ptr<gfx::shader> {
            auto asset = _shader_assets.at(name);
            auto& renderer = scene.get_renderer();
            if (renderer.shaders().contains(asset->name()))
            {
                return renderer.shaders().at(asset->name());
            }
            return renderer.create_shader(asset->name(), asset->source(), asset->type());
        };

        result.texture_loader =
            [this](const std::string& name, const engine::scene& scene) -> std::shared_ptr<engine::texture> {
            auto asset = _texture_assets.at(name);
            auto mips = asset->load_mips();

            engine::texture_args_from_data tex_args;
            tex_args.name = asset->name();
            tex_args.sampler_info = asset->sampler_info();
            tex_args.format = asset->format();
            tex_args.mips = asset->load_mips();
            tex_args.size = { asset->width(), asset->height() };

            return scene.get_renderer().create_color_texture_from_data(tex_args);
        };

        return result;
    }

    std::vector<std::string> project::available_scenes() const
    {
        std::vector<std::string> result;
        for (const auto& entry : std::filesystem::recursive_directory_iterator(_scenes_path))
        {
            if (!entry.is_regular_file() || (ien::str_tolower(ien::get_file_extension(entry.path())) != ".cscene"))
            {
                continue;
            }
            result.push_back(abspath_to_name<engine::scene>(entry.path().string()));
        }
        return result;
    }

    void project::save_scene(const engine::scene& scene, const std::string& name) const
    {
        std::stringstream sstr;
        {
            cereal::JSONOutputArchive archive(sstr);
            archive(scene);
        }
        std::filesystem::create_directories(_scenes_path);
        ien::write_file_text((std::filesystem::path(_scenes_path) / name).string(), sstr.str());
    }

    engine::scene project::load_scene(const std::string& name, cathedral::engine::renderer* renderer) const
    {
        const auto abs_path = name_to_abspath<engine::scene>(name);

        CRITICAL_CHECK(ien::get_file_extension(abs_path) == SCENE_FILE_EXT, "Invalid scene file extension");

        std::ifstream ifs(abs_path);
        cereal::JSONInputArchive archive(ifs);

        engine::scene_args args;
        args.loaders = get_loader_funcs();
        args.prenderer = renderer;

        engine::scene scene(args);
        archive(scene);

        return scene;
    }

    project project::create(const std::string& path, const std::string& name)
    {
        const auto project_path = std::filesystem::path(path);
        const auto project_file_path = project_path / ".cathedral";
        ien::write_file_text(project_file_path.string(), std::format("project-name:{}", name));

        std::filesystem::create_directories(project_path / "materials");
        std::filesystem::create_directories(project_path / "meshes");
        std::filesystem::create_directories(project_path / "shaders");
        std::filesystem::create_directories(project_path / "textures");
        std::filesystem::create_directories(project_path / "scenes");

        project result;
        const auto load_result = result.load_project(path);
        CRITICAL_CHECK(load_result == load_project_status::OK, "Failure loading project");

        return result;
    }

    template <concepts::Asset TAsset>
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
                target_container.emplace(ast->name(), ast);
            }
        }
    }

    void project::load_shader_assets()
    {
        load_assets(_shaders_path, _shader_assets);
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