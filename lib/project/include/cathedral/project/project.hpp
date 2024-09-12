#pragma once

#include <cathedral/project/assets/material_asset.hpp>
#include <cathedral/project/assets/material_definition_asset.hpp>
#include <cathedral/project/assets/mesh_asset.hpp>
#include <cathedral/project/assets/shader_asset.hpp>
#include <cathedral/project/assets/texture_asset.hpp>

#include <ien/str_utils.hpp>

#include <filesystem>
#include <string>

namespace cathedral::project
{
    enum class load_project_status
    {
        OK,
        PROJECT_PATH_NOT_FOUND,
        PROJECT_FILE_NOT_FOUND,
        PROJECT_FILE_READ_FAILURE
    };

    class project
    {
    public:
        load_project_status load_project(const std::string& project_path);

        bool is_loaded() const { return _loaded; }

        const std::string& root_path() const { return _root_path; }

        const std::string& shaders_path() const { return _shaders_path; }

        const std::string& material_definitions_path() const { return _material_definitions_path; }

        const std::string& materials_path() const { return _materials_path; }

        const std::string& textures_path() const { return _textures_path; }

        const std::string& meshes_path() const { return _meshes_path; }

        template <AssetLike TAsset>
        void add_asset(std::shared_ptr<TAsset> asset)
        {
            get_asset_map<TAsset>().emplace(asset->path(), asset);
        }

        const auto& shader_assets() const { return _shader_assets; }

        const auto& material_definition_assets() const { return _material_definition_assets; }

        const auto& texture_assets() const { return _texture_assets; }

        const auto& material_assets() const { return _material_assets; }

        void reload_shader_assets();
        void reload_material_definition_assets();
        void reload_texture_assets();
        void reload_material_assets();

        template <AssetLike TAsset>
        void reload_assets()
        {
            if constexpr (std::is_same_v<TAsset, shader_asset>)
                return reload_shader_assets();
            else if constexpr (std::is_same_v<TAsset, material_definition_asset>)
                return reload_material_definition_assets();
            else if constexpr (std::is_same_v<TAsset, texture_asset>)
                return reload_texture_assets();
            else if constexpr (std::is_same_v<TAsset, material_asset>)
                return reload_material_assets();

            CRITICAL_ERROR("Unhandled asset type");
        }

        template <AssetLike TAsset>
        std::shared_ptr<TAsset> get_asset_by_path(const std::string& path)
        {
            return get_asset_map<TAsset>().at(path);
        }

        template <AssetLike TAsset>
        std::shared_ptr<TAsset> get_asset_by_relative_path(const std::string& path)
        {
            CRITICAL_CHECK(!path.empty());
            return get_asset_by_path<TAsset>((std::filesystem::path(get_assets_path<TAsset>()) / path).string());
        }

        template <AssetLike TAsset>
        std::shared_ptr<TAsset> get_asset_by_relative_name(const std::string& name)
        {
            CRITICAL_CHECK(!name.empty());
            return get_asset_by_path<TAsset>((std::filesystem::path(get_assets_path<TAsset>()) / name).string() + ".casset");
        }

        template <AssetLike TAsset>
        const auto& get_assets()
        {
            return get_asset_map<TAsset>();
        }

        template <AssetLike TAsset>
        const std::string& get_assets_path() const
        {
            if constexpr (std::is_same_v<TAsset, shader_asset>)
                return _shaders_path;
            else if constexpr (std::is_same_v<TAsset, material_definition_asset>)
                return _material_definitions_path;
            else if constexpr (std::is_same_v<TAsset, texture_asset>)
                return _textures_path;
            else if constexpr (std::is_same_v<TAsset, material_asset>)
                return _materials_path;
            else if constexpr (std::is_same_v<TAsset, mesh_asset>)
                return _meshes_path;

            CRITICAL_ERROR("Unhandled asset type");
        }

        std::string name_to_relpath(const std::string& name) const { return name + ".casset"; }

        template <AssetLike TAsset>
        std::string name_to_abspath(const std::string& name) const
        {
            return (std::filesystem::path(get_assets_path<TAsset>()) / name).string() + ".casset";
        }

        std::string relpath_to_name(const std::string& relpath) const
        {
            return relpath.substr(0, relpath.size() - (sizeof(".casset") - 1));
        }

        template <AssetLike TAsset>
        std::string relpath_to_abspath(const std::string& relpath) const
        {
            return (std::filesystem::path(get_assets_path<TAsset>()) / relpath).string();
        }

        std::string abspath_to_name(const std::string& abspath)
        {
            const auto pathstr = std::filesystem::path(abspath).filename().string();
            const auto result = ien::str_replace(pathstr, ".casset", "", pathstr.size() - sizeof(".casset"));
            return result;
        }

    private:
        bool _loaded = false;
        std::string _project_name;
        std::string _root_path;

        std::string _materials_path;
        std::string _material_definitions_path;
        std::string _meshes_path;
        std::string _shaders_path;
        std::string _textures_path;

        std::unordered_map<std::string, std::shared_ptr<material_asset>> _material_assets;
        std::unordered_map<std::string, std::shared_ptr<material_definition_asset>> _material_definition_assets;
        std::unordered_map<std::string, std::shared_ptr<mesh_asset>> _mesh_assets;
        std::unordered_map<std::string, std::shared_ptr<shader_asset>> _shader_assets;
        std::unordered_map<std::string, std::shared_ptr<texture_asset>> _texture_assets;

        template <AssetLike TAsset>
        std::unordered_map<std::string, std::shared_ptr<TAsset>>& get_asset_map()
        {
            if constexpr (std::is_same_v<TAsset, shader_asset>)
                return _shader_assets;
            else if constexpr (std::is_same_v<TAsset, material_definition_asset>)
                return _material_definition_assets;
            else if constexpr (std::is_same_v<TAsset, texture_asset>)
                return _texture_assets;
            else if constexpr (std::is_same_v<TAsset, material_asset>)
                return _material_assets;
            else if constexpr (std::is_same_v<TAsset, mesh_asset>)
                return _mesh_assets;

            CRITICAL_ERROR("Unhandled asset type");
        }

        template <AssetLike TAsset>
        void load_assets(const std::string& path, std::unordered_map<std::string, std::shared_ptr<TAsset>>& target_container);

        void load_shader_assets();
        void load_material_definition_assets();
        void load_texture_assets();
        void load_material_assets();
        void load_mesh_assets();
    };
} // namespace cathedral::project