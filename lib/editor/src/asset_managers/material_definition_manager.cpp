#include <cathedral/editor/asset_managers/material_definition_manager.hpp>

#include <cathedral/editor/common/message.hpp>

#include "ui_material_definition_manager.h"

namespace cathedral::editor
{
    material_definition_manager::material_definition_manager(project::project& pro, QWidget* parent)
        : QMainWindow(parent)
        , _project(pro)
        , _ui(new Ui::material_definition_manager())
    {
        _ui->setupUi(this);

        connect(_ui->pushButton_CustomTypes, &QPushButton::clicked, this, [this]{
            show_error_message("Not implemented", this);
        });
    }
}