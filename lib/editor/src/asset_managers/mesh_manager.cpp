#include <cathedral/editor/asset_managers/mesh_manager.hpp>

#include "ui_mesh_manager.h"

namespace cathedral::editor
{
    mesh_manager::mesh_manager(project::project& pro, QWidget* parent)
        : QMainWindow(parent)
        , resource_manager_base(pro)
        , _ui(new Ui::mesh_manager)
    {
        _ui->setupUi(this);
    }
} // namespace cathedral::editor