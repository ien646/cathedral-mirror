#include <QApplication>
#include <QStyle>
#include <QStyleFactory>
#include <QStyleHints>

#include <ien/platform.hpp>

#include <cathedral/editor/editor_window.hpp>
#include <cathedral/editor/styling.hpp>
#include <cathedral/editor/welcome_dialog.hpp>

using namespace cathedral;

int main(int argc, char** argv)
{
#ifdef IEN_OS_WIN
    std::string project_path = "C:\\Users\\Ien\\Documents\\cathedral\\test-project";
#else
    std::string project_path = "/home/ien/Projects/cathedral/test-project";
#endif

#ifndef IEN_OS_WIN
    qputenv("QT_QPA_PLATFORM", "xcb");
#endif

    CATHEDRAL_EDITOR_INITIALIZE();

    QApplication qapp(argc, argv);

    QApplication::setPalette(editor::get_editor_palette());
    QApplication::setStyle(editor::get_editor_style());
    qapp.setStyleSheet(editor::get_editor_stylesheet());

    QApplication::setFont(editor::get_editor_font());

    auto* welcome_window = new editor::welcome_dialog();
    if (welcome_window->exec() == 0)
    {
        return 0;
    }
    auto project = welcome_window->project();
    delete welcome_window;

    auto* win = new editor::editor_window(project);
    win->show();

    QApplication::processEvents();
    win->initialize_vulkan();

    auto& renderer = win->renderer();
    auto& scene = win->scene();

    win->swapchain().set_present_mode(vk::PresentModeKHR::eMailbox);

    QApplication::processEvents();
    while (true)
    {
        QApplication::processEvents();

        if (!win->isVisible())
        {
            return 0;
        }
        scene.tick([&](double deltatime) {});
    }
}