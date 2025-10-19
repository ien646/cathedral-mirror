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
        void tick();

        using callback_t = std::function<void()>;

        struct
        {
            callback_t new_project, open_project, close, new_scene, open_scene, save_scene, save_as_scene, fonts, materials,
                meshes, shaders, textures, capture_screenshot, reset_layout, text_scale_up, text_scale_down;
        } callbacks;
    };
} // namespace cathedral::editor2