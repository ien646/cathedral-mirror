#pragma once

#include <cathedral/project/assets/material_definition_asset.hpp>
#include <cathedral/project/assets/shader_asset.hpp>

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

        void add_asset(std::shared_ptr<shader_asset> asset);
        void add_asset(std::shared_ptr<material_definition_asset> asset);

        const auto& shader_assets() const { return _shader_assets; }
        const auto& material_definition_assets() const { return _material_definition_assets; }

        void reload_shader_assets();
        void reload_material_definition_assets();

        template <AssetLike TAsset>
        std::shared_ptr<TAsset> get_asset_by_path(const std::string& path)
        {
            return get_asset_map<TAsset>().at(path);
        }

    private:
        bool _loaded = false;
        std::string _project_name;
        std::string _root_path;
        std::string _shaders_path;
        std::string _material_definitions_path;
        std::string _materials_path;
        std::string _textures_path;

        std::unordered_map<std::string, std::shared_ptr<shader_asset>> _shader_assets;
        std::unordered_map<std::string, std::shared_ptr<material_definition_asset>> _material_definition_assets;

        template <AssetLike TAsset>
        std::unordered_map<std::string, std::shared_ptr<TAsset>>& get_asset_map()
        {
            if constexpr (std::is_same_v<TAsset, shader_asset>)
            {
                return _shader_assets;
            }
            else if constexpr (std::is_same_v<TAsset, material_definition_asset>)
            {
                return _material_definition_assets;
            }
            CRITICAL_ERROR("Unhandled asset type");
        }

        template <AssetLike TAsset, typename TContainer>
        void load_assets(const std::string& path, TContainer& target_container);

        void load_shader_assets();
        void load_material_definition_assets();
    };
} // namespace cathedral::project