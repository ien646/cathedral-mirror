#include <cathedral/bits/log.hpp>

namespace cathedral
{
    void log_database::log_info(std::string msg)
    {
        std::lock_guard lock(_mux);
        _lines.emplace_back(log_level::INFO, std::move(msg));
    }

    void log_database::log_warning(std::string msg)
    {
        std::lock_guard lock(_mux);
        _lines.emplace_back(log_level::WARNING, std::move(msg));
    }

    void log_database::log_error(std::string msg)
    {
        std::lock_guard lock(_mux);
        _lines.emplace_back(log_level::ERROR, std::move(msg));
    }

    std::vector<log_line> log_database::take_log_lines()
    {
        std::lock_guard lock(_mux);
        std::vector<log_line> result = std::move(_lines);
        _lines = {};
        return result;
    }

    log_database _logdb;

    log_database& get_global_log_database()
    {
        return _logdb;
    }
} // namespace cathedral