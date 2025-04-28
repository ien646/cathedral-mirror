#pragma once

#include <cathedral/core.hpp>

#include <cathedral/engine/default_resources.hpp>

#include <memory>

FORWARD_CLASS(cathedral::engine, material);
FORWARD_CLASS(cathedral::engine, mesh3d_node);
FORWARD_CLASS(cathedral::engine, renderer);
FORWARD_CLASS(cathedral::engine, scene);

namespace cathedral::editor
{
    std::shared_ptr<engine::mesh3d_node> get_translation_gizmo_node(engine::scene& scene);
}