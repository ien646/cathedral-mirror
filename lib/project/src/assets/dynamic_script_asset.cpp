#include <cathedral/project/assets/dynamic_script_asset.hpp>

#include <cathedral/project/project.hpp>

#include <ien/io_utils.hpp>

namespace cathedral::project
{
    void dynamic_script_asset::save() const
    {
        ien::write_file_text(_path, _source);
    }

    void dynamic_script_asset::load()
    {
        const auto read_result = ien::read_file_text(_path);
        CRITICAL_CHECK(read_result.has_value(), "Failed to read script file");
        _source = *read_result;
    }

    std::string dynamic_script_asset::relative_path() const
    {
        return _project->abspath_to_relpath<dynamic_script_asset>(_path);
    }

    std::string dynamic_script_asset::name() const
    {
        return _project->abspath_to_name<dynamic_script_asset>(_path);
    }
} // namespace cathedral::project