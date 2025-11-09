#pragma once

#include <cathedral/core.hpp>

#include <functional>
#include <optional>

FORWARD_CLASS(cathedral::project, project);

namespace cathedral::editor
{
    class scene_selector_dialog
    {
    public:
        void tick(const project::project& project);

        void open();

        struct
        {
            std::function<void(std::string)> selected;
        } callbacks;

    private:
        bool _open_flag = false;
        std::optional<std::string> _selected;
    };
} // namespace cathedral::editor