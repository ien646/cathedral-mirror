#pragma once

#include <cathedral/editor/callback_decl.hpp>
#include <cathedral/editor/settings.hpp>
#include <cathedral/event_bus.hpp>

#include <functional>

FORWARD_CLASS(cathedral::project, project);
FORWARD_CLASS(cathedral, settings);

namespace cathedral::editor
{
    class editor_window_menubar
    {
    public:
        explicit editor_window_menubar(event_bus& bus);
        void tick() const;

    private:
        event_bus& _event_bus;
    };

    namespace events
    {
        CATHEDRAL_DECLARE_EVENTS(
            ,
            new_project,
            open_project,
            settings_dialog_open,
            close_editor,
            new_scene,
            open_scene,
            save_scene,
            save_as_scene,
            font_manager_open,
            material_manager_open,
            mesh_manager_open,
            script_manager_open,
            shader_manager_open,
            texture_manager_open,
            capture_screenshot,
            editor_window_reset_layout,
            global_text_scale_up,
            global_text_scale_down);
    }
} // namespace cathedral::editor