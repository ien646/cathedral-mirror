#pragma once

#include <QMainWindow>

namespace Ui
{
    class material_manager;
};

namespace cathedral::project
{
    class project;
}

namespace cathedral::editor
{
    class material_manager : QMainWindow
    {
        Q_OBJECT

    public:
        material_manager(project::project& pro, QWidget* parent);

    private:
        Ui::material_manager* _ui;
        project::project& _project;
    };
} // namespace cathedral::editor