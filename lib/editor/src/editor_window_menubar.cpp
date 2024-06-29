#include <cathedral/editor/editor_window_menubar.hpp>

namespace cathedral::editor
{
    editor_window_menubar::editor_window_menubar(QWidget* parent)
        : QMenuBar(parent)
    {
        auto file_menu = addMenu("File");
        auto close_action = file_menu->addAction("Close");
        connect(close_action, &QAction::triggered, this, [this](bool checked) { emit close_clicked(); });

        auto help_menu = addMenu("Help");
        auto about_action = help_menu->addAction("About");
        connect(about_action, &QAction::triggered, this, [this](bool checked) { emit about_clicked(); });
    }
} // namespace cathedral::editor