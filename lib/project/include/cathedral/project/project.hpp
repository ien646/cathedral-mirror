#pragma once

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
        const std::string& materials_path() const { return _materials_path; }

        void add_asset(std::shared_ptr<shader_asset> asset);

        const auto& shader_assets() const { return _shader_assets; }

    private:
        bool _loaded = false;
        std::string _project_name;
        std::string _root_path;
        std::string _shaders_path;
        std::string _materials_path;

        std::unordered_map<std::string, std::shared_ptr<shader_asset>> _shader_assets;

        template<typename TAsset, typename TContainer>
            requires(std::is_base_of_v<asset, TAsset>)
        void load_assets(const std::string& path, TContainer& target_container);

        void load_shader_assets();
        void load_material_assets();
    };
} // namespace cathedral::project