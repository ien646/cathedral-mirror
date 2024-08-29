#pragma once

#include <QMainWindow>

namespace cathedral::project
{
    class project;
}

namespace Ui
{
    class material_manager;
}

namespace cathedral::editor
{
    class material_manager : public QMainWindow
    {
        Q_OBJECT

    public:
        material_manager(project::project& pro, QWidget* parent);

    private:
        project::project& _project;
        Ui::material_manager* _ui = nullptr;

    private slots:
        
    };
} // namespace cathedral::editor