#include <cathedral/editor/asset_managers/material_manager.hpp>

#include <cathedral/editor/asset_managers/dialogs/new_material_dialog.hpp>
#include <cathedral/editor/common/text_input_dialog.hpp>

#include <cathedral/core.hpp>

#include <cathedral/project/assets/material_asset.hpp>
#include <cathedral/project/project.hpp>

#include <ien/str_utils.hpp>

#include <QComboBox>
#include <QFormLayout>

#include "ui_material_manager.h"

#include <filesystem>

namespace cathedral::editor
{
    material_manager::material_manager(project::project& pro, QWidget* parent)
        : QMainWindow(parent)
        , resource_manager_base(pro)
        , _ui(new Ui::material_manager)
    {
        _ui->setupUi(this);

        init_shaders_tab();

        connect(_ui->itemManagerWidget, &item_manager::add_clicked, this, &SELF::slot_add_material_clicked);
        connect(_ui->itemManagerWidget, &item_manager::rename_clicked, this, &SELF::slot_rename_material_clicked);
        connect(_ui->itemManagerWidget, &item_manager::delete_clicked, this, &SELF::slot_delete_material_clicked);

        reload();
    }

    item_manager* material_manager::get_item_manager_widget()
    {
        return _ui->itemManagerWidget;
    }

    void material_manager::init_shaders_tab()
    {
        QStringList vx_shader_list;
        QStringList fg_shader_list;
        for (const auto& [path, shader] : _project.shader_assets())
        {
            const auto relpath = QString::fromStdString(shader->relative_path());
            switch (shader->type())
            {
            case gfx::shader_type::VERTEX:
                vx_shader_list << relpath;
                break;
            case gfx::shader_type::FRAGMENT:
                fg_shader_list << relpath;
            default:
                CRITICAL_ERROR("Unhandled shader type");
            }
        }

        auto* vxsh_combo = new QComboBox;
        vxsh_combo->addItems(vx_shader_list);

        auto* fgsh_combo = new QComboBox;
        fgsh_combo->addItems(fg_shader_list);

        auto* shaders_layout = new QFormLayout;
        shaders_layout->addRow("Vertex shader: ", vxsh_combo);
        shaders_layout->addRow("Fragment shader: ", fgsh_combo);

        _ui->tab_Shaders->setLayout(shaders_layout);
    }

    void material_manager::slot_add_material_clicked()
    {
        QStringList matdefs;
        for (const auto& [path, asset] : _project.get_assets<project::material_definition_asset>())
        {
            matdefs << QString::fromStdString(asset->relative_path());
        }

        auto* diag = new new_material_dialog(_ui->itemManagerWidget->get_texts(), matdefs, this);
        if (diag->exec() == QDialog::DialogCode::Accepted)
        {
            const auto path = (std::filesystem::path(get_assets_path()) / diag->name().toStdString()).string() + ".casset";
            auto new_asset = std::make_shared<project::material_asset>(_project, path);
            new_asset->set_material_definition_ref(diag->matdef().toStdString());
            new_asset->mark_as_manually_loaded();
            new_asset->save();

            _project.add_asset(new_asset);
            reload();
        }
    }

    void material_manager::slot_rename_material_clicked()
    {
        rename_asset();
    }

    void material_manager::slot_delete_material_clicked()
    {
        delete_asset();
    }
} // namespace cathedral::editor