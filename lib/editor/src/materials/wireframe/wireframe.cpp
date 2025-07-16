#include <cathedral/editor/materials/wireframe.hpp>

#include <cathedral/engine/material.hpp>
#include <cathedral/engine/renderer.hpp>

#include "battery/embed.hpp"

namespace cathedral::editor
{
    namespace
    {
        constexpr auto NAME = "__cathedral_editor_wireframe__";
    } // namespace

    std::weak_ptr<engine::material> get_wireframe_material(engine::renderer& renderer)
    {
        engine::material_args args;
        args.name = NAME;
        args.domain = engine::material_domain::OVERLAY;
        args.material_bindings = {};
        args.node_bindings = { { "node_model", engine::shader_node_uniform_binding::NODE_MODEL_MATRIX } };
        args.vertex_shader_source = b::embed<"editor/shaders/wireframe/vertex.glsl">().str();
        args.fragment_shader_source = b::embed<"editor/shaders/wireframe/fragment.glsl">().str();

        return renderer.create_material(std::move(args));
    }
} // namespace cathedral::editor