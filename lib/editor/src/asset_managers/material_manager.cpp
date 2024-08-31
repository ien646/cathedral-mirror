#include <cathedral/editor/asset_managers/material_manager.hpp>

#include "ui_material_manager.h"

namespace cathedral::editor
{
    material_manager::material_manager(project::project& pro, QWidget* parent)
        : QMainWindow(parent)
        , _project(pro)
        , _ui(new Ui::material_manager())
    {
        _ui->setupUi(this);
    }
}