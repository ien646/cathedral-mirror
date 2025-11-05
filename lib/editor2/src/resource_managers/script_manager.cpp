#include <cathedral/editor2/resource_managers/script_manager.hpp>

namespace cathedral::editor2
{
    script_manager::script_manager(project::project& pro)
        : resource_manager_base(pro)
    {
    }

    void script_manager::tick()
    {
        if (_window.keep_open())
        {
            _scene->tick([this]([[maybe_unused]] const double deltatime) { _window.tick([this] { tick_gui(); }); });
        }
    }

    void script_manager::tick_gui()
    {

    }
} // namespace cathedral::editor2