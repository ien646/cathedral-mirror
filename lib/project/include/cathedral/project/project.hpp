#pragma once

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
        const std::string& shaders_path() const;

    private:
        bool _loaded = false;
        std::string _project_name;
        std::string _root_path;
        std::string _shaders_path;
    };
}