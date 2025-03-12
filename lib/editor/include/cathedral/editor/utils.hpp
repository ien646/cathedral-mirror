#pragma once

#include <QString>

#include <string>

namespace cathedral::editor
{
    inline QString to_q_string(const std::string& str)
    {
        return QString::fromStdString(str);
    }

    inline QString to_q_string(std::string_view strview)
    {
        return QString::fromStdString(std::string{ strview });
    }

    template <typename... Args>
    inline QString to_q_string(std::string_view format, const Args&... args)
    {
        return QString::fromStdString(std::vformat(format, std::make_format_args(args...)));
    }

    template <typename... Args>
    inline QString QSTR(const Args&... v) //NOLINT
    {
        return to_q_string(v...);
    }
} // namespace cathedral::editor