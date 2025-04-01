#include <cathedral/editor/editor_window_menubar.hpp>

#include <cathedral/editor/asset_managers/shader_manager.hpp>

#include <QFileDialog>

namespace cathedral::editor
{
    editor_window_menubar::editor_window_menubar(QWidget* parent)
        : QMenuBar(parent)
    {
        auto* file_menu = addMenu("File");
        {
            auto* open_project_action = file_menu->addAction("Open Project...");
            connect(open_project_action, &QAction::triggered, this, &SELF::open_project_clicked);

            auto* close_action = file_menu->addAction("Close");
            connect(close_action, &QAction::triggered, this, &SELF::close_clicked);
        }

        auto* scene_menu = addMenu("Scene");
        {
            auto* new_action = scene_menu->addAction("New...");
            connect(new_action, &QAction::triggered, this, &SELF::new_scene_clicked);

            auto* open_action = scene_menu->addAction("Open...");
            connect(open_action, &QAction::triggered, this, &SELF::open_scene_clicked);

            auto* save_action = scene_menu->addAction("Save...");
            connect(save_action, &QAction::triggered, this, &SELF::save_scene_clicked);
        }

        auto* resources_menu = addMenu("Resources");
        {
            auto* materials_action = resources_menu->addAction("Materials...");
            connect(materials_action, &QAction::triggered, this, &SELF::material_manager_clicked);

            auto* meshes_action = resources_menu->addAction("Meshes...");
            connect(meshes_action, &QAction::triggered, this, &SELF::mesh_manager_clicked);

            auto* shaders_action = resources_menu->addAction("Shaders...");
            connect(shaders_action, &QAction::triggered, this, &SELF::shader_manager_clicked);

            auto* textures_action = resources_menu->addAction("Textures...");
            connect(textures_action, &QAction::triggered, this, &SELF::texture_manager_clicked);
        }

        auto* tools_menu = addMenu("Tools");
        {
            auto* capture_action = tools_menu->addAction("Capture screenshot");
            connect(capture_action, &QAction::triggered, this, &SELF::capture_clicked);
        }

        auto* help_menu = addMenu("Help");
        {
            auto* about_action = help_menu->addAction("About");
            connect(about_action, &QAction::triggered, this, &SELF::about_clicked);
        }
    }
} // namespace cathedral::editor