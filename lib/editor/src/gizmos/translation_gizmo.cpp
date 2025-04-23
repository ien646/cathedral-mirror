#include <cathedral/editor/gizmos/translation_gizmo.hpp>

#include <cathedral/engine/material.hpp>
#include <cathedral/engine/renderer.hpp>

namespace cathedral::editor
{
    constexpr const char* MATERIAL_NAME = "__cathedral_translation_gizmo__";

    const engine::mesh& get_translation_gizmo_mesh()
    {
        return engine::gizmos::get_translation_gizmo_mesh();
    }

    const std::weak_ptr<engine::material> cathedral::editor::get_translation_gizmo_material(engine::renderer& renderer)
    {
        const auto generate_material = [](engine::renderer& renderer) -> std::weak_ptr<engine::material> {
            engine::material_args args;
            args.domain = engine::material_domain::OVERLAY;
            args.name = MATERIAL_NAME;
            args.vertex_shader_source = engine::gizmos::get_translation_gizmo_vertex_shader();
            args.fragment_shader_source = engine::gizmos::get_translation_gizmo_fragment_shader();

            return renderer.create_material(args);
        };

        if (renderer.materials().contains(MATERIAL_NAME))
        {
            return renderer.materials().at(MATERIAL_NAME);
        }
        else
        {
            return generate_material(renderer);
        }
    }
}