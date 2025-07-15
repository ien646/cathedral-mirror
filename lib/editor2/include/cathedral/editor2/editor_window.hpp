#pragma once

#include "widget_registry.hpp"

#include <cathedral/project/project.hpp>

#include <cathedral/core.hpp>

#include <cathedral/editor2/dialogs/error_dialog.hpp>
#include <cathedral/editor2/dialogs/open_scene_dialog.hpp>
#include <cathedral/editor2/dialogs/text_input_dialog.hpp>
#include <cathedral/editor2/menubar.hpp>
#include <cathedral/engine/renderer.hpp>
#include <cathedral/sdl/keyboard.hpp>
#include <cathedral/sdl/mouse.hpp>
#include <cathedral/sdl/window.hpp>

#include <memory>

namespace cathedral::engine
{
    class renderer;
}

namespace cathedral::sdl
{
    class window;
}

namespace cathedral::editor2
{
    class editor_window final
    {
    public:
        explicit editor_window();

        void tick();
        bool should_close() const;

    private:
        bool _should_close = false;
        std::unique_ptr<gfx::vulkan_context> _vkctx;
        std::unique_ptr<gfx::swapchain> _swapchain;
        std::unique_ptr<sdl::window> _window;
        std::unique_ptr<engine::renderer> _renderer;
        std::unique_ptr<project::project> _project;
        std::shared_ptr<engine::scene> _scene;
        std::shared_ptr<sdl::keyboard_input> _keyboard_input;
        std::shared_ptr<sdl::mouse_input> _mouse_input;

        widget_registry _widget_registry;

        std::shared_ptr<error_dialog> _error_dialog;
        std::shared_ptr<menubar> _menubar;
        std::shared_ptr<text_input_dialog> _new_scene_dialog;
        std::shared_ptr<open_scene_dialog> _open_scene_dialog;
        std::shared_ptr<text_input_dialog> _save_scene_as_dialog;

        void init_imgui();
        void init_ui();

        void init_new_scene_dialog(const std::vector<std::string>& available_scenes);
        void init_open_scene_dialog();
        void init_save_as_scene_dialog();

        void handle_new_project();
        void handle_open_project();

        void handle_scene_save();
        void handle_scene_save_as();
    };
} // namespace cathedral::editor2