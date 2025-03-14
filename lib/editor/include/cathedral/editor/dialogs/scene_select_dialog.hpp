#pragma once

#include <cathedral/core.hpp>

#include <QDialog>

FORWARD_CLASS(cathedral::project, project);

namespace cathedral::editor
{
    class scene_select_dialog : public QDialog
    {
    public:
        scene_select_dialog(project::project& pro, QWidget* parent);

        const std::string& selected_scene() const;

    private:
        project::project& _project;
        std::string _selected_scene;
    };
}