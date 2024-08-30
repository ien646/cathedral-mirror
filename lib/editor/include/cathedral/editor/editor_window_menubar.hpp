#pragma once

#include <QMenuBar>
#include <QWidget>

namespace cathedral::editor
{
    class editor_window_menubar : public QMenuBar
    {
        Q_OBJECT

    public:
        editor_window_menubar(QWidget* parent = nullptr);

    signals:
        // File
        void open_project_clicked();
        void close_clicked();

        // Resources
        void texture_manager_clicked();
        void shader_manager_clicked();
        void material_definition_manager_clicked();
        void material_manager_clicked();

        // Help
        void about_clicked();
    };
} // namespace cathedral::editor