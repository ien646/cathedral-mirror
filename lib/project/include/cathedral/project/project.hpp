#pragma once

#include <cathedral/project/assets/material_asset.hpp>
#include <cathedral/project/assets/mesh_asset.hpp>
#include <cathedral/project/assets/shader_asset.hpp>
#include <cathedral/project/assets/texture_asset.hpp>

#include <ien/str_utils.hpp>

#include <filesystem>
#include <string>

FORWARD_CLASS(cathedral::engine, renderer);
FORWARD_CLASS(cathedral::engine, scene);
FORWARD_STRUCT(cathedral::engine, scene_loader_funcs);
FORWARD_CLASS(cathedral::engine, upload_queue);

namespace cathedral::project
{
    enum class load_project_status : uint8_t
    {
        OK,
        PROJECT_PATH_NOT_FOUND,
        PROJECT_FILE_NOT_FOUND,
        PROJECT_FILE_READ_FAILURE
    };

    namespace concepts
    {
        template <typename T>
        concept AssetOrScene = Asset<T> || std::is_same_v<T, engine::scene>;
    }

    constexpr const char* ASSET_FILE_EXT = ".casset";
    constexpr const char* SCENE_FILE_EXT = ".cscene";

    class project
    {
    public:
        [[nodiscard]] load_project_status load_project(const std::string& project_path);

        bool is_loaded() const { return _loaded; }

        const std::string& root_path() const { return _root_path; }

        const std::string& shaders_path() const { return _shaders_path; }

        const std::string& materials_path() const { return _materials_path; }

        const std::string& textures_path() const { return _textures_path; }

        const std::string& meshes_path() const { return _meshes_path; }

        const std::string& scenes_path() const { return _scenes_path; }

        template <concepts::Asset TAsset>
        void add_asset(std::shared_ptr<TAsset> asset)
        {
            get_asset_map<TAsset>().emplace(asset->name(), asset);
        }

        const auto& shader_assets() const { return _shader_assets; }

        const auto& texture_assets() const { return _texture_assets; }

        const auto& material_assets() const { return _material_assets; }

        const auto& mesh_assets() const { return _mesh_assets; }

        void reload_shader_assets();
        void reload_texture_assets();
        void reload_material_assets();
        void reload_mesh_assets();

        template <concepts::Asset TAsset>
        void reload_assets()
        {
            if constexpr (std::is_same_v<TAsset, shader_asset>)
            {
                reload_shader_assets();
            }
            else if constexpr (std::is_same_v<TAsset, texture_asset>)
            {
                reload_texture_assets();
            }
            else if constexpr (std::is_same_v<TAsset, material_asset>)
            {
                reload_material_assets();
            }
            else if constexpr (std::is_same_v<TAsset, mesh_asset>)
            {
                reload_mesh_assets();
            }
            else
            {
                CRITICAL_ERROR("Unhandled asset type");
            }
        }

        template <concepts::Asset TAsset>
        std::shared_ptr<TAsset> get_asset_by_path(const std::string& path)
        {
            return get_asset_map<TAsset>().at(abspath_to_name<TAsset>(path));
        }

        template <concepts::Asset TAsset>
        std::shared_ptr<TAsset> get_asset_by_relative_path(const std::string& path)
        {
            CRITICAL_CHECK(!path.empty(), "Invalid empty path");
            return get_asset_by_path<TAsset>((std::filesystem::path(get_assets_path<TAsset>()) / path).string());
        }

        template <concepts::Asset TAsset>
        std::shared_ptr<TAsset> get_asset_by_name(const std::string& name) const
        {
            CRITICAL_CHECK(!name.empty(), "Invalid empty path");
            return get_assets<TAsset>().at(name);
        }

        template <concepts::Asset TAsset>
        const auto& get_assets() const
        {
            return get_asset_map<TAsset>();
        }

        template <concepts::AssetOrScene TAsset>
        const std::string& get_assets_path() const
        {
            if constexpr (std::is_same_v<TAsset, shader_asset>)
            {
                return _shaders_path;
            }
            else if constexpr (std::is_same_v<TAsset, texture_asset>)
            {
                return _textures_path;
            }
            else if constexpr (std::is_same_v<TAsset, material_asset>)
            {
                return _materials_path;
            }
            else if constexpr (std::is_same_v<TAsset, mesh_asset>)
            {
                return _meshes_path;
            }
            else if constexpr (std::is_same_v<TAsset, engine::scene>)
            {
                return _scenes_path;
            }

            CRITICAL_ERROR("Unhandled asset type");
        }

        const std::string& get_assets_path_by_typestr(std::string_view typestr) const
        {
            if (typestr == get_asset_typestr<shader_asset>())
            {
                return _shaders_path;
            }
            if (typestr == get_asset_typestr<texture_asset>())
            {
                return _textures_path;
            }
            if (typestr == get_asset_typestr<material_asset>())
            {
                return _materials_path;
            }
            if (typestr == get_asset_typestr<mesh_asset>())
            {
                return _meshes_path;
            }
            CRITICAL_ERROR("Unhandled asset typestr");
        }

        std::string name_to_relpath(const std::string& name) const { return name + ASSET_FILE_EXT; }

        template <concepts::AssetOrScene TAsset>
        std::string name_to_abspath(const std::string& name) const
        {
            return (std::filesystem::path(get_assets_path<TAsset>()) / name).string() +
                   (concepts::Asset<TAsset> ? ASSET_FILE_EXT : SCENE_FILE_EXT);
        }

        std::string relpath_to_name(const std::string& relpath) const
        {
            constexpr size_t ASSET_EXT_SIZE = sizeof(ASSET_FILE_EXT) - 1;
            return relpath.substr(0, relpath.size() - ASSET_EXT_SIZE);
        }

        template <concepts::AssetOrScene T>
        std::string relpath_to_abspath(const std::string& relpath) const
        {
            return (std::filesystem::path(get_assets_path<T>()) / relpath).string();
        }

        template <concepts::AssetOrScene T>
        std::string abspath_to_relpath(const std::string& abspath) const
        {
            const auto& assets_path = get_assets_path<T>();
            CRITICAL_CHECK(abspath.starts_with(assets_path), "Absolute path does not match project path");
            return abspath.substr(assets_path.size() + 1, std::string::npos);
        }

        template <concepts::AssetOrScene T>
        std::string abspath_to_name(const std::string& abspath) const
        {
            return relpath_to_name(abspath_to_relpath<T>(abspath));
        }

        engine::scene_loader_funcs get_loader_funcs() const;

        std::vector<std::string> available_scenes() const;

        void save_scene(const engine::scene& scene, const std::string& name) const;

        engine::scene load_scene(const std::string& name, cathedral::engine::renderer* renderer) const;

        static project create(const std::string& path, const std::string& name);

    private:
        bool _loaded = false;
        std::string _project_name;
        std::string _root_path;

        std::string _materials_path;
        std::string _material_definitions_path;
        std::string _meshes_path;
        std::string _shaders_path;
        std::string _textures_path;

        std::string _scenes_path;

        std::unordered_map<std::string, std::shared_ptr<material_asset>> _material_assets;
        std::unordered_map<std::string, std::shared_ptr<mesh_asset>> _mesh_assets;
        std::unordered_map<std::string, std::shared_ptr<shader_asset>> _shader_assets;
        std::unordered_map<std::string, std::shared_ptr<texture_asset>> _texture_assets;

        template <concepts::Asset TAsset>
        const std::unordered_map<std::string, std::shared_ptr<TAsset>>& get_asset_map() const
        {
            if constexpr (std::is_same_v<TAsset, shader_asset>)
            {
                return _shader_assets;
            }
            if constexpr (std::is_same_v<TAsset, texture_asset>)
            {
                return _texture_assets;
            }
            if constexpr (std::is_same_v<TAsset, material_asset>)
            {
                return _material_assets;
            }
            if constexpr (std::is_same_v<TAsset, mesh_asset>)
            {
                return _mesh_assets;
            }
            CRITICAL_ERROR("Unhandled asset type");
        }

        template <concepts::Asset TAsset>
        std::unordered_map<std::string, std::shared_ptr<TAsset>>& get_asset_map()
        {
            if constexpr (std::is_same_v<TAsset, shader_asset>)
            {
                return _shader_assets;
            }
            if constexpr (std::is_same_v<TAsset, texture_asset>)
            {
                return _texture_assets;
            }
            if constexpr (std::is_same_v<TAsset, material_asset>)
            {
                return _material_assets;
            }
            if constexpr (std::is_same_v<TAsset, mesh_asset>)
            {
                return _mesh_assets;
            }

            CRITICAL_ERROR("Unhandled asset type");
        }

        template <concepts::Asset TAsset>
        void load_assets(const std::string& path, std::unordered_map<std::string, std::shared_ptr<TAsset>>& target_container);

        void load_shader_assets();
        void load_texture_assets();
        void load_material_assets();
        void load_mesh_assets();
    };
} // namespace cathedral::project