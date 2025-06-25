#include <cathedral/script/log.hpp>

#include <cathedral/core.hpp>

constexpr auto ANNOTATIONS = R"(

---@type fun(name:string)
function log_info(name) end

---@type fun(name:string)
function log_warning(name) end

---@type fun(name:string)
function log_error(name) end

)";

namespace cathedral::script
{
    void log_initializer::initialize(state& s)
    {
        s.set_function("log_error", [&s](const std::string& message) {
            log_error(std::format("[script '{}'] {}", s.get<std::string>("__cathedral_script__"), message));
        });
        s.set_function("log_warning", [&s](const std::string& message) {
            log_warning(std::format("[script '{}']: {}", s.get<std::string>("__cathedral_script__"), message));
        });
        s.set_function("log_info", [&s](const std::string& message) {
            log_info(std::format("[script '{}']: {}", s.get<std::string>("__cathedral_script__"), message));
        });
    }

    const std::string& log_initializer::get_annotations()
    {
        return ANNOTATIONS;
    }
} // namespace cathedral::script