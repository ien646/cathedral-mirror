#pragma once

#include <cathedral/core.hpp>

#include <QMainWindow>

FORWARD_CLASS(cathedral::project, project);
FORWARD_CLASS(Ui, mesh_manager);

namespace cathedral::editor
{
    class mesh_manager : public QMainWindow
    {
    public:
        mesh_manager(project::project& pro, QWidget* parent);

    private:
        project::project& _project;
        Ui::mesh_manager* _ui = nullptr;
    };
} // namespace cathedral::editor