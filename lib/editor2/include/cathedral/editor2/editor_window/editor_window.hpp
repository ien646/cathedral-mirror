#pragma once

#include <cathedral/editor2/editor_window/menubar.hpp>
#include <cathedral/editor2/editor_window/scene_tree.hpp>
#include <cathedral/editor2/editor_window/viewport.hpp>
#include <cathedral/editor2/engine_window.hpp>
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
        editor_window_menubar _menubar;
        scene_tree _scene_tree;
        editor_viewport _viewport;
        std::shared_ptr<project::project> _project;
        std::shared_ptr<engine::scene> _scene;
    };
} // namespace cathedral::editor2