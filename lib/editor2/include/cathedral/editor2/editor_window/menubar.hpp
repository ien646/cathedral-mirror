#pragma once

#include <cathedral/editor2/settings.hpp>

#include <functional>

FORWARD_CLASS(cathedral::project, project);
FORWARD_CLASS(cathedral, settings);

namespace cathedral::editor2
{
    class editor_window_menubar
    {
    public:
        void tick(const editor_settings_interface& settings);

        using callback_t = std::function<void()>;

        struct
        {
            callback_t new_project, open_project, close, new_scene, open_scene, save_scene, save_as_scene, fonts, materials,
                meshes, shaders, textures, capture_screenshot, settings_changed;
        } callbacks;
    };
} // namespace cathedral::editor2