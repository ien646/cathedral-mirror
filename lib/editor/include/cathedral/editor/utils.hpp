#pragma once

#include <QString>

#include <string>

namespace cathedral::editor
{
    inline QString toQString(const std::string& str)
    {
        return QString::fromStdString(str);
    }

    inline QString toQString(std::string_view strview)
    {
        return QString::fromStdString(std::string{ strview });
    }

    template <typename... Args>
    inline QString toQString(std::string_view format, const Args&... args)
    {
        return QString::fromStdString(std::vformat(format, std::make_format_args(args...)));
    }

    template <typename... Args>
    inline QString QSTR(const Args&... v)
    {
        return toQString(v...);
    }
} // namespace cathedral::editor