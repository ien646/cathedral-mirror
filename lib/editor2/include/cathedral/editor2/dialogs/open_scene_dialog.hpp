#pragma once

#include <cathedral/project/project.hpp>

#include <cathedral/editor2/widget.hpp>

#include <functional>

namespace cathedral::editor2
{
    struct open_scene_dialog_callbacks
    {
        std::function<void(std::string)> selected;
    };

    class open_scene_dialog final : public widget<open_scene_dialog_callbacks>
    {
    public:
        explicit open_scene_dialog(project::project& pro);
        void tick() override;

        void open();

    private:
        project::project& _project;
        std::string _selected;
        bool _open = false;
        bool _first_open = false;
    };
} // namespace cathedral::editor2