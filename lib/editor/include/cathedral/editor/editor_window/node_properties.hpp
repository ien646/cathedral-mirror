#pragma once

#include <cathedral/ds.hpp>
#include <cathedral/editor/dialogs/list_select_dialog.hpp>
#include <cathedral/editor/dialogs/mesh_selector.hpp>
#include <cathedral/editor/dialogs/texture_selector.hpp>

FORWARD_CLASS(cathedral::engine, scene_node);
FORWARD_CLASS(cathedral::engine, node);
FORWARD_CLASS(cathedral::engine, camera2d_node);
FORWARD_CLASS(cathedral::engine, camera3d_node);
FORWARD_CLASS(cathedral::engine, directional_light_node);
FORWARD_CLASS(cathedral::engine, mesh3d_node);
FORWARD_CLASS(cathedral::engine, point_light_node);

namespace cathedral::editor
{
    class node_properties
    {
    public:
        explicit node_properties(project::project& project);

        void tick(engine::scene& scene, const unordered_set<engine::scene_node*>& nodes);

        static constexpr auto WINDOW_ID = "Node properties";

    private:
        project::project& _project;
        std::unique_ptr<list_select_dialog> _material_selector_dialog;
        std::unique_ptr<mesh_selector> _mesh_selector_dialog;
        std::unique_ptr<list_select_dialog> _script_selector_dialog;
        std::unique_ptr<texture_selector> _texture_selector_dialog;

        void draw_snode_scripts_section(engine::scene_node* snode);
        void draw_node_transform(engine::node* node);
        void draw_camera2d_properties(engine::camera2d_node* node);
        void draw_camera3d_properties(engine::camera3d_node* node);
        void draw_dirlight_properties(engine::directional_light_node* node);
        void draw_mesh3d_properties(engine::scene& scene, engine::mesh3d_node* node);
        void draw_pointlight_properties(engine::point_light_node* node);
    };
} // namespace cathedral::editor