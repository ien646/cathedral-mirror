#include <cathedral/editor/editor_nodes/gizmos/translation_gizmo.hpp>

#include <cathedral/engine/material.hpp>
#include <cathedral/engine/renderer.hpp>
#include <cathedral/engine/scene.hpp>
#include <cathedral/engine/scene_node.hpp>

#include <cathedral/engine/nodes/mesh3d_node.hpp>

namespace cathedral::editor
{
    constexpr auto NAME = "__cathedral_translation_gizmo__";

    namespace
    {
        const engine::mesh& get_translation_gizmo_mesh()
        {
            return engine::gizmos::get_translation_gizmo_mesh();
        }

        std::weak_ptr<engine::material> get_translation_gizmo_material(engine::renderer& renderer)
        {
            if (renderer.materials().contains(NAME))
            {
                return renderer.materials().at(NAME);
            }

            engine::material_args args;
            args.domain = engine::material_domain::OVERLAY;
            args.name = NAME;
            args.vertex_shader_source = engine::gizmos::get_translation_gizmo_vertex_shader();
            args.fragment_shader_source = engine::gizmos::get_translation_gizmo_fragment_shader();
            args.node_bindings[engine::shader_node_uniform_binding::NODE_MODEL_MATRIX] = "model_matrix";

            return renderer.create_material(args);
        }
    } // namespace

    std::shared_ptr<engine::mesh3d_node> get_translation_gizmo_node(engine::scene& scene)
    {
        if (scene.contains_node(NAME))
        {
            return std::dynamic_pointer_cast<engine::mesh3d_node>(scene.get_node(NAME));
        }

        auto node = std::make_shared<engine::mesh3d_node>(NAME, nullptr, false);

        // Generate material in renderer
        std::ignore = get_translation_gizmo_material(scene.get_renderer());

        node->set_material(NAME);
        node->set_mesh(scene.get_mesh_buffers(NAME, get_translation_gizmo_mesh()));
        node->disable();
        node->set_hidden_in_editor(true);
        node->set_local_rotation({ 0.0F, 180.0F, 0.0F });
        node->set_local_scale({ 1.0F, 1.0F, -1.0F });

        scene.add_root_node(node);

        return node;
    }
} // namespace cathedral::editor