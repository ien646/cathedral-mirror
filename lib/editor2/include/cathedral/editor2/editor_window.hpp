#pragma once

#include "dialogs/open_scene_dialog.hpp"

#include <cathedral/project/project.hpp>

#include <cathedral/core.hpp>

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

        std::unique_ptr<menubar> _menubar;
        std::unique_ptr<open_scene_dialog> _open_scene_dialog;

        void init_imgui();
        void init_ui();
    };
} // namespace cathedral::editor2