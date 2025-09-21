#pragma once

#include <cathedral/core.hpp>
#include <cathedral/engine/nodes/mesh3d_node.hpp>

#include <memory>

FORWARD_CLASS(cathedral::engine, material);
FORWARD_CLASS(cathedral::engine, renderer);
FORWARD_CLASS(cathedral::engine, scene);

namespace cathedral::editor
{
    class translation_gizmo final : public engine::mesh3d_node
    {

    };

    engine::mesh3d_node* get_translation_gizmo_node(engine::scene& scene);
} // namespace cathedral::editor