#pragma once

#include "cathedral/editor2/resource_managers/mesh_manager.hpp"

#include <cathedral/editor2/dialogs/confirm_dialog.hpp>
#include <cathedral/editor2/dialogs/message_dialog.hpp>
#include <cathedral/editor2/dialogs/scene_selector_dialog.hpp>
#include <cathedral/editor2/dialogs/text_input_dialog.hpp>
#include <cathedral/editor2/editor_window/logs_panel.hpp>
#include <cathedral/editor2/editor_window/menubar.hpp>
#include <cathedral/editor2/editor_window/node_properties.hpp>
#include <cathedral/editor2/editor_window/scene_tree.hpp>
#include <cathedral/editor2/editor_window/stats_panel.hpp>
#include <cathedral/editor2/editor_window/viewport.hpp>
#include <cathedral/editor2/engine_window.hpp>
#include <cathedral/editor2/resource_managers/font_manager.hpp>
#include <cathedral/editor2/resource_managers/material_manager.hpp>
#include <cathedral/project/project.hpp>

namespace cathedral::editor2
{
    class editor_window
    {
    public:
        explicit editor_window(std::shared_ptr<project::project> project);

        int execute();

    private:
        std::unique_ptr<engine_window> _window;
        std::unordered_set<engine::scene_node*> _selected_nodes;
        editor_window_menubar _menubar;
        scene_tree _scene_tree;
        node_properties _node_properties;
        editor_viewport _viewport;
        logs_panel _logs_panel;
        stats_panel _stats_panel;
        std::shared_ptr<project::project> _project;
        std::shared_ptr<engine::scene> _scene;
        bool _skip_gui_flag = false;

        std::vector<std::function<void()>> _pre_tick_callbacks;
        std::vector<std::function<void()>> _post_tick_callbacks;
        void enqueue_pre_tick_action(std::function<void()> pre_tick_callback);
        void enqueue_post_tick_action(std::function<void()> post_tick_callback);

        std::unique_ptr<font_manager> _font_manager;
        std::unique_ptr<material_manager> _material_manager;
        std::unique_ptr<mesh_manager> _mesh_manager;

        struct
        {
            text_input_dialog new_scene{ "New scene", "Name" };
            text_input_dialog save_as_scene{ "Save scene", "Name" };
        } _input_dialogs;

        confirm_dialog _confirm_dialog{ "Confirm", "placeholder" };
        message_dialog _message_dialog;
        scene_selector_dialog _scene_selector_dialog;

        void hide_ui_for_this_frame();

        void init_inputs();
        void init_menubar_callbacks();
    };
} // namespace cathedral::editor2