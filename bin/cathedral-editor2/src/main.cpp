#include <cathedral/editor2/editor_window/editor_window.hpp>
#include <cathedral/editor2/engine_window.hpp>
#include <cathedral/editor2/project_selection_window.hpp>
#include <cathedral/memory.hpp>
#include <cathedral/project/project.hpp>

using namespace cathedral;

std::shared_ptr<project::project> load_project()
{
    init_scratch_memory();

    std::shared_ptr<project::project> project = std::make_shared<project::project>();

#ifdef CATHEDRAL_APP_EDITOR2_INITIAL_PROJECT_DIR
    const auto load_result = project->load_project(CATHEDRAL_APP_EDITOR2_INITIAL_PROJECT_DIR);
    CRITICAL_CHECK(
        load_result == project::load_project_status::OK,
        std::format("Unable to load project at '{}'", CATHEDRAL_APP_EDITOR2_INITIAL_PROJECT_DIR));
#else
    bool project_selected = false;
    while (!project_selected)
    {
        const std::optional<std::string> project_path = editor2::project_selection_window{}.execute();
        if (!project_path.has_value())
        {
            return {};
        }
        switch (project->load_project(*project_path))
        {
        case project::load_project_status::OK:
            project_selected = true;
            break;
        case project::load_project_status::PROJECT_PATH_NOT_FOUND:
        case project::load_project_status::PROJECT_FILE_NOT_FOUND:
        case project::load_project_status::PROJECT_FILE_READ_FAILURE:
        default:
            break;
        }
    }
#endif

    return project;
}

int main()
{
#ifdef CATHEDRAL_LINUX_PLATFORM_X11
    setenv("SDL_VIDEO_DRIVER", "x11", 1);
#endif
    auto project = load_project();
    if (project == nullptr)
    {
        return 0;
    }

    editor2::editor_window editor_window(std::move(project));
    return editor_window.execute();
}