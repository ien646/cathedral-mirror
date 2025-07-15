#include <cathedral/editor2/editor_window.hpp>

#include <cathedral/memory.hpp>

int main(int argc, char* argv[])
{
    cathedral::init_scratch_memory();

#ifdef CATHEDRAL_LINUX_PLATFORM_X11
    setenv("SDL_VIDEODRIVER", "x11", 1);
#endif

    cathedral::editor2::editor_window editor_window;

    while (!editor_window.should_close())
    {
        editor_window.tick();
        cathedral::flush_scratch_memory();
    }
}