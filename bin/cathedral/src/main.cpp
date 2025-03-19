#include <QApplication>
#include <QStyle>
#include <QStyleFactory>
#include <QStyleHints>

#include <ien/platform.hpp>

#include <cathedral/editor/editor_window.hpp>
#include <cathedral/editor/styling.hpp>
#include <cathedral/editor/utils.hpp>
#include <cathedral/editor/welcome_dialog.hpp>

using namespace cathedral;

int main(int argc, char** argv)
{
#ifndef IEN_OS_WIN
    qputenv("QT_QPA_PLATFORM", "xcb");
#endif

    CATHEDRAL_EDITOR_INITIALIZE();

    QApplication qapp(argc, argv);

    QApplication::setPalette(editor::get_editor_palette());
    QApplication::setStyle(editor::get_editor_style());
    qapp.setStyleSheet(editor::get_editor_stylesheet());

    QApplication::setFont(editor::get_editor_font());

    std::shared_ptr<project::project> project = {};
    if (std::filesystem::exists(std::filesystem::current_path() / "../../../test-project"))
    {
        project = std::make_shared<project::project>();
        const auto load_result = project->load_project("./../../../test-project");
        CRITICAL_CHECK(load_result == project::load_project_status::OK);
    }
    else
    {
        auto* welcome_window = new editor::welcome_dialog();
        if (welcome_window->exec() == 0)
        {
            return 0;
        }
        auto project = welcome_window->project();
        delete welcome_window;
    }

    auto* win = new editor::editor_window(project);
    win->show();

    QApplication::processEvents();
    win->initialize_vulkan();

    win->swapchain().set_present_mode(vk::PresentModeKHR::eMailbox);

    QApplication::processEvents();
    while (true)
    {
        QApplication::processEvents();

        if (!win->isVisible())
        {
            return 0;
        }
        win->scene()->tick([&](double deltatime) {
            const auto fps = 1.0 / deltatime;
            win->set_status_text(editor::QSTR("FPS: {:.2f}", fps));
        });
    }
}