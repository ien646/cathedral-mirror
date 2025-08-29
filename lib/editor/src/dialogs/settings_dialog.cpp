#include <cathedral/editor/dialogs/settings_dialog.hpp>

namespace cathedral::editor
{
    settings_dialog::settings_dialog(project::project& pro, QWidget* parent)
        : QDialog(parent)
        , _project(pro)
    {
    }
} // namespace cathedral::editor