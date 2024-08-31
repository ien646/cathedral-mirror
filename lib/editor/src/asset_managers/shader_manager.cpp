#include <cathedral/editor/asset_managers/shader_manager.hpp>

#include "ui_shader_manager.h"

namespace cathedral::editor
{
    shader_manager::shader_manager()
        : _ui(new Ui::shader_manager())
    {
        _ui->setupUi(this);
    }
}