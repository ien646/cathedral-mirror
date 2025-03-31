#pragma once

#include <cathedral/gfx/shader.hpp>

#include <string>

namespace cathedral::engine
{
    std::string preprocess_shader(gfx::shader_type type, std::string_view source, bool skip_replace = false);
}