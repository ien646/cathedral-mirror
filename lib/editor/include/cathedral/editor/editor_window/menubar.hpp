#pragma once

#include <cathedral/editor/callback_decl.hpp>
#include <cathedral/editor/settings.hpp>

#include <functional>

FORWARD_CLASS(cathedral::project, project);
FORWARD_CLASS(cathedral, settings);

namespace cathedral::editor
{
    class editor_window_menubar
    {
    public:
        void tick() const;

        using callback_t = std::function<void()>;

        CATHEDRAL_DECLARE_CALLBACKS(
            (new_project, void),
            (open_project, void),
            (settings, void),
            (close, void),
            (new_scene, void),
            (open_scene, void),
            (save_scene, void),
            (save_as_scene, void),
            (fonts, void),
            (materials, void),
            (meshes, void),
            (scripts, void),
            (shaders, void),
            (textures, void),
            (capture_screenshot, void),
            (reset_layout, void),
            (text_scale_up, void),
            (text_scale_down, void));
    };
} // namespace cathedral::editor