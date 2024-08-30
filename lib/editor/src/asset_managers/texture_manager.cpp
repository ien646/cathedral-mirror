#include <cathedral/editor/asset_managers/texture_manager.hpp>

#include "ui_texture_manager.h"

namespace cathedral::editor
{
    texture_manager::texture_manager(project::project& pro, QWidget* parent)
        : QMainWindow(parent)
        , _ui(new Ui::texture_manager)
        , _project(pro)
    {
        _ui->setupUi(this);

        connect(_ui->actionClose, &QAction::triggered, this, &texture_manager::close);
    }
}