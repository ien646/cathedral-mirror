#pragma once

#include <cathedral/editor2/engine_window.hpp>

namespace cathedral::editor2
{
    class project_selection_window
    {
    public:
        project_selection_window();

        std::optional<std::string> execute();

    private:
        engine_window _window;
    };
} // namespace cathedral::editor2