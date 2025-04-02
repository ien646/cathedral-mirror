#pragma once

#include <cathedral/core.hpp>

#include <expected>

FORWARD_CLASS(cathedral::gfx, shader);

namespace cathedral::engine
{
    std::optional<std::string> validate_shader(const gfx::shader& shader);
}