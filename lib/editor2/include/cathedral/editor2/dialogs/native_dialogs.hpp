#pragma once

#include <optional>
#include <string>
#include <vector>

namespace cathedral::editor2
{
    std::optional<std::string> file_select_dialog(const std::vector<std::pair<std::string, std::string>>& filters = {});
    std::optional<std::string> directory_select_dialog();
}