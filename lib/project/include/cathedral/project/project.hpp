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

        void add_asset(std::shared_ptr<shader_asset> asset);

        const std::vector<std::shared_ptr<shader_asset>> shader_assets() const { return _shader_assets; }

    private:
        bool _loaded = false;
        std::string _project_name;
        std::string _root_path;
        std::string _shaders_path;

        std::vector<std::shared_ptr<shader_asset>> _shader_assets;

        void load_shader_assets();
    };
} // namespace cathedral::project