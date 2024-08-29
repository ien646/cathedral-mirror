#pragma once

#include <QMainWindow>

namespace cathedral::project
{
    class project;
}

namespace Ui
{
    class material_definition_manager;
}

namespace cathedral::editor
{
    class material_definition_manager : public QMainWindow
    {
        Q_OBJECT

    public:
        material_definition_manager(project::project& pro, QWidget* parent);

    private:
        project::project& _project;
        Ui::material_definition_manager* _ui = nullptr;

    private slots:
        
    };
} // namespace cathedral::editor