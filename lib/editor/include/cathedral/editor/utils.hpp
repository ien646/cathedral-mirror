#pragma once

#include <ien/platform.hpp>

#include <QString>

#include <string>

namespace cathedral::editor
{
    inline QString to_q_string(const std::string& str)
    {
        return QString::fromStdString(str);
    }

    inline QString to_q_string(const std::string_view strview)
    {
        return QString::fromStdString(std::string{ strview });
    }

    inline QString to_q_string(const char* str)
    {
        return QString{ str };
    }

    template <typename... Args>
    QString to_q_string(const std::string_view format, const Args&... args)
    {
        return QString::fromStdString(std::vformat(format, std::make_format_args(args...)));
    }

    template <typename... Args>
    QString QSTR(const Args&... v)
    {
        return to_q_string(v...);
    }

#if defined(IEN_OS_WIN)
    inline std::vector<const char*> get_vulkan_instance_extensions()
    {
        return { "VK_KHR_win32_surface" };
    }
#elif defined(IEN_OS_LINUX)
    inline std::vector<const char*> get_vulkan_instance_extensions()
    {
        if (qgetenv("QT_QPA_PLATFORM") == "xcb")
        {
            return { "VK_KHR_xcb_surface" };
        }
        return { "VK_KHR_wayland_surface" };
    }
#else
    #error "Unsupported platform!"
#endif
} // namespace cathedral::editor