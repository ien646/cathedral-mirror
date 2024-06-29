#include <SDL2/SDL.h>

#include <QApplication>
#include <QStyle>
#include <QStyleFactory>
#include <QVulkanInstance>
#include <QWindow>

#include <ien/platform.hpp>

#include <cathedral/editor/editor_window.hpp>
#include <cathedral/gfx/vulkan_context.hpp>

using namespace cathedral;

int main(int argc, char** argv)
{
    QApplication qapp(argc, argv);
    editor::editor_window* win = new editor::editor_window();
    win->show();
    while(true)
    {
        QApplication::processEvents();
        win->tick();
    }
}