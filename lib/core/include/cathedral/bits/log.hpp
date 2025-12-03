#pragma once

#include <cathedral/bits/move.hpp>

#include <mutex>
#include <vector>

namespace cathedral
{
    enum class log_level
    {
        INFO,
        WARNING,
        ERROR
    };

    struct log_line
    {
        log_level level = log_level::INFO;
        std::string message;
    };

    class log_database
    {
    public:
        void log_info(std::string msg);
        void log_warning(std::string msg);
        void log_error(std::string msg);

        std::vector<log_line> take_log_lines();

    private:
        std::vector<log_line> _lines;
        std::mutex _mux;
    };

    log_database& get_global_log_database();

    inline void log_info(std::string msg)
    {
        get_global_log_database().log_info(MOVE(msg));
    }

    inline void log_warning(std::string msg)
    {
        get_global_log_database().log_warning(MOVE(msg));
    }

    inline void log_error(std::string msg)
    {
        get_global_log_database().log_error(MOVE(msg));
    }

} // namespace cathedral