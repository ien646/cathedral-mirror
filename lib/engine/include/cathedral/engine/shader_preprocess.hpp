#pragma once

#include <expected>
#include <string>

namespace cathedral::engine
{
    std::expected<std::string, std::string> preprocess_shader(std::string_view source);
}