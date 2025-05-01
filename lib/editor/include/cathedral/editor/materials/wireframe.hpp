#pragma once

#include <cathedral/core.hpp>

FORWARD_CLASS(cathedral::engine, material);
FORWARD_CLASS(cathedral::engine, renderer);

namespace cathedral::editor
{
    std::weak_ptr<engine::material> get_wireframe_material(engine::renderer& renderer);
}