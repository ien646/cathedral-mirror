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

    CATHEDRAL_DECLARE_EMPTY_SCRUCTS(
        new_project_event,
        open_project_event,
        settings_dialog_open_event,
        close_editor_event,
        new_scene_event,
        open_scene_event,
        save_scene_event,
        save_as_scene_event,
        font_manager_open_event,
        material_manager_open_event,
        mesh_manager_open_event,
        script_manager_open_event,
        shader_manager_open_event,
        texture_manager_open_event,
        capture_screenshot_event,
        editor_window_reset_layout_event,
        global_text_scale_up_event,
        global_text_scale_down_event);
} // namespace cathedral::editor