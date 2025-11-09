#pragma once

#include "cathedral/editor/settings.hpp"

#include <cathedral/core.hpp>

FORWARD_CLASS(cathedral::project, project);

namespace cathedral::editor
{
    class settings_dialog
    {
    public:
        void open();

        void tick(project::project& pro, const editor_settings_interface& editor_settings);

    private:
        one_time_flag _open_flag;
    };
} // namespace cathedral::editor