#pragma once

#include <cathedral/core.hpp>

#include <cathedral/engine/default_resources.hpp>

#include <memory>

FORWARD_CLASS(cathedral::engine, material);
FORWARD_CLASS(cathedral::engine, renderer);
FORWARD_CLASS(cathedral::engine, scene);
FORWARD_CLASS(cathedral::engine, scene_node);

namespace cathedral::editor
{
    std::shared_ptr<engine::scene_node> get_translation_gizmo_node(engine::scene& scene);
}