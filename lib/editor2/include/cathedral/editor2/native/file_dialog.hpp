#pragma once

#include <optional>
#include <string>

namespace cathedral::editor2
{
    std::optional<std::string> native_open_file();
    std::optional<std::string> native_open_dir();
}