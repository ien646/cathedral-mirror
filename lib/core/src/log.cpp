#include <cathedral/bits/log.hpp>

namespace cathedral
{
    namespace
    {
        log_database logdb;
    }

    void log_database::log_info(std::string msg)
    {
        const std::lock_guard lock(_mux);
        _lines.emplace_back(log_level::INFO, MOVE(msg));
    }

    void log_database::log_warning(std::string msg)
    {
        const std::lock_guard lock(_mux);
        _lines.emplace_back(log_level::WARNING, MOVE(msg));
    }

    void log_database::log_error(std::string msg)
    {
        const std::lock_guard lock(_mux);
        _lines.emplace_back(log_level::ERROR, MOVE(msg));
    }

    std::vector<log_line> log_database::take_log_lines()
    {
        const std::lock_guard lock(_mux);
        std::vector<log_line> result = MOVE(_lines);
        _lines = {};
        return result;
    }

    log_database& get_global_log_database()
    {
        return logdb;
    }
} // namespace cathedral