#include <cathedral/script/log.hpp>

#include <cathedral/core.hpp>

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
} // namespace cathedral::script