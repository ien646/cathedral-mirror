#pragma once

#include <cathedral/core.hpp>

#include <cathedral/engine/default_resources.hpp>

FORWARD_CLASS(cathedral::engine, material);
FORWARD_CLASS(cathedral::engine, renderer);

namespace cathedral::editor
{
    const engine::mesh& get_translation_gizmo_mesh();
    const std::weak_ptr<engine::material> get_translation_gizmo_material(engine::renderer& renderer);
}