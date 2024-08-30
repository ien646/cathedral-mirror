#include <cathedral/editor/editor_window_menubar.hpp>

#include <cathedral/editor/asset_managers/shader_manager.hpp>

#include <QFileDialog>

namespace cathedral::editor
{
    editor_window_menubar::editor_window_menubar(QWidget* parent)
        : QMenuBar(parent)
    {
        auto file_menu = addMenu("File");
        {
            auto open_project_action = file_menu->addAction("Open Project...");
            connect(open_project_action, &QAction::triggered, this, &editor_window_menubar::open_project_clicked);

            auto close_action = file_menu->addAction("Close");
            connect(close_action, &QAction::triggered, this, &editor_window_menubar::close_clicked);
        }

        auto resources_menu = addMenu("Resources");
        {
            auto shaders_action = resources_menu->addAction("Manage shaders...");
            connect(shaders_action, &QAction::triggered, this, &editor_window_menubar::shader_manager_clicked);

            auto material_defs_action = resources_menu->addAction("Manage material definitions...");
            connect(material_defs_action, &QAction::triggered, this, &editor_window_menubar::material_definition_manager_clicked);
            
            auto materials_action = resources_menu->addAction("Manage materials...");
            connect(materials_action, &QAction::triggered, this, &editor_window_menubar::material_manager_clicked);
        }

        auto help_menu = addMenu("Help");
        {
            auto about_action = help_menu->addAction("About");
            connect(about_action, &QAction::triggered, this, &editor_window_menubar::about_clicked);
        }
    }
} // namespace cathedral::editor