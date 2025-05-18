#include <QApplication>
#include <QStyle>
#include <QStyleFactory>
#include <QStyleHints>

#include <ien/circular_array.hpp>
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
    // qapp.setStyleSheet(editor::get_editor_stylesheet());

    QApplication::setFont(editor::get_editor_font());

    std::shared_ptr<project::project> project = {};
    if (const auto current_path = std::filesystem::current_path();
        std::filesystem::exists(current_path / "../../../test-project"))
    {
        project = std::make_shared<project::project>();
        const auto load_result = project->load_project("./../../../test-project");
        CRITICAL_CHECK(load_result == project::load_project_status::OK, "Failure loading project");
    }
    else if (std::filesystem::exists(current_path / "../../../../../test-project"))
    {
        project = std::make_shared<project::project>();
        const auto load_result = project->load_project("../../../../../test-project");
        CRITICAL_CHECK(load_result == project::load_project_status::OK, "Failure loading project");
    }
    else
    {
        auto* welcome_window = new editor::welcome_dialog();
        if (welcome_window->exec() == 0)
        {
            return 0;
        }
        project = welcome_window->project();
        delete welcome_window;
    }

    auto* win = new editor::editor_window(project);
    win->show();

    // Pass all QApplication events through the editor_window event filter, so that
    // keyboard and mouse events can be checked without having to deal with widget focus
    qapp.installEventFilter(win);

    QApplication::processEvents();
    win->initialize_vulkan();

    win->swapchain().set_present_mode(vk::PresentModeKHR::eImmediate);

    double deltatime_accum = 1.0;
    ien::circular_array<double, 10> deltatime_smooth;

    QApplication::processEvents();
    win->scene()->set_in_editor_mode(true);
    while (true)
    {
        QApplication::processEvents();

        if (!win->isVisible())
        {
            return 0;
        }
        win->tick([&](const double deltatime) {
            deltatime_accum += deltatime;
            deltatime_smooth.push(deltatime);
            if (deltatime_accum >= 1.0)
            {
                deltatime_accum = 0.0;
                const auto fps =
                    1.0 / (std::ranges::fold_left(deltatime_smooth.underlying_array(), 0.0, std::plus<double>()) /
                           deltatime_smooth.size());
                win->set_status_text(editor::QSTR("FPS: {:.1f}", fps));
            }
        });
    }
}