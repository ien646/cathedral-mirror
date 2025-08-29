#include <cathedral/editor/editor_window_menubar.hpp>

#include <cathedral/editor/asset_managers/shader_manager.hpp>

#include <QFileDialog>

namespace cathedral::editor
{
    editor_window_menubar::editor_window_menubar(QWidget* parent)
        : QMenuBar(parent)
    {
        setObjectName("editor_window_menubar");

        {
            auto* file_menu = addMenu("File");
            const auto* open_project_action = file_menu->addAction("Open Project...");
            connect(open_project_action, &QAction::triggered, this, &SELF::open_project_clicked);

            const auto* settings_action = file_menu->addAction("Settings...");
            connect(settings_action, &QAction::triggered, this, &SELF::settings_clicked);

            const auto* close_action = file_menu->addAction("Close");
            connect(close_action, &QAction::triggered, this, &SELF::close_clicked);
        }

        {
            auto* scene_menu = addMenu("Scene");
            const auto* new_action = scene_menu->addAction("New...");
            connect(new_action, &QAction::triggered, this, &SELF::new_scene_clicked);

            const auto* open_action = scene_menu->addAction("Open...");
            connect(open_action, &QAction::triggered, this, &SELF::open_scene_clicked);

            const auto* save_action = scene_menu->addAction("Save...");
            connect(save_action, &QAction::triggered, this, &SELF::save_scene_clicked);

            const auto* save_as_action = scene_menu->addAction("Save as...");
            connect(save_as_action, &QAction::triggered, this, &SELF::save_as_scene_clicked);
        }

        {
            auto* resources_menu = addMenu("Resources");

            const auto* fonts_action = resources_menu->addAction("Fonts...");
            connect(fonts_action, &QAction::triggered, this, &SELF::font_manager_clicked);

            const auto* materials_action = resources_menu->addAction("Materials...");
            connect(materials_action, &QAction::triggered, this, &SELF::material_manager_clicked);

            const auto* meshes_action = resources_menu->addAction("Meshes...");
            connect(meshes_action, &QAction::triggered, this, &SELF::mesh_manager_clicked);

            const auto* shaders_action = resources_menu->addAction("Shaders...");
            connect(shaders_action, &QAction::triggered, this, &SELF::shader_manager_clicked);

            const auto* textures_action = resources_menu->addAction("Textures...");
            connect(textures_action, &QAction::triggered, this, &SELF::texture_manager_clicked);

            const auto* scripts_action = resources_menu->addAction("Scripts...");
            connect(scripts_action, &QAction::triggered, this, &SELF::script_manager_clicked);
        }

        {
            auto* tools_menu = addMenu("Tools");
            const auto* capture_action = tools_menu->addAction("Capture screenshot");
            connect(capture_action, &QAction::triggered, this, &SELF::capture_clicked);
        }

        {
            auto* help_menu = addMenu("Help");
            const auto* about_action = help_menu->addAction("About");
            connect(about_action, &QAction::triggered, this, &SELF::about_clicked);
        }
    }
} // namespace cathedral::editor