#pragma once

#include <cathedral/engine/scene.hpp>
#include <cathedral/editor/engine_window.hpp>

namespace cathedral::editor
{
    class project_selection_window
    {
    public:
        project_selection_window();

        std::optional<std::string> execute();

    private:
        engine_window _window;
        std::unique_ptr<engine::scene> _scene;
    };
} // namespace cathedral::editor