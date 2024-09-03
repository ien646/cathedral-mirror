#include <cathedral/editor/asset_managers/material_manager.hpp>

#include <cathedral/editor/asset_managers/dialogs/new_material_dialog.hpp>
#include <cathedral/editor/common/text_input_dialog.hpp>
#include <cathedral/editor/common/texture_list_widget.hpp>
#include <cathedral/editor/common/texture_picker_dialog.hpp>
#include <cathedral/editor/common/texture_slot_widget.hpp>
#include <cathedral/editor/texture_utils.hpp>

#include <cathedral/engine/default_resources.hpp>
#include <cathedral/engine/texture_decompression.hpp>

#include <cathedral/core.hpp>

#include <cathedral/project/assets/material_asset.hpp>
#include <cathedral/project/project.hpp>

#include <ien/str_utils.hpp>

#include <QComboBox>
#include <QFormLayout>
#include <QLabel>
#include <QShowEvent>
#include <QtConcurrent>

#include <magic_enum.hpp>

#include "ui_material_manager.h"

namespace cathedral::editor
{
    material_manager::material_manager(project::project& pro, QWidget* parent)
        : QMainWindow(parent)
        , resource_manager_base(pro)
        , _ui(new Ui::material_manager)
    {
        _ui->setupUi(this);

        connect(_ui->itemManagerWidget, &item_manager::add_clicked, this, &SELF::slot_add_material_clicked);
        connect(_ui->itemManagerWidget, &item_manager::rename_clicked, this, &SELF::slot_rename_material_clicked);
        connect(_ui->itemManagerWidget, &item_manager::delete_clicked, this, &SELF::slot_delete_material_clicked);
        connect(_ui->itemManagerWidget, &item_manager::item_selection_changed, this, &SELF::slot_material_selection_changed);
    }

    item_manager* material_manager::get_item_manager_widget()
    {
        return _ui->itemManagerWidget;
    }

    void material_manager::reload_material_props()
    {
        init_shaders_tab();
        init_textures_tab();
    }

    void material_manager::init_shaders_tab()
    {
        if (!_ui->tab_Shaders->layout())
        {
            _ui->tab_Shaders->setLayout(new QFormLayout);
        }
        else if (_ui->tab_Shaders->layout()->count() > 0)
        {
            while (QLayoutItem* child = _ui->tab_Shaders->layout()->takeAt(0))
            {
                delete child->widget();
                delete child;
            }
        }

        QStringList vx_shader_list;
        QStringList fg_shader_list;
        for (const auto& [path, shader] : _project.shader_assets())
        {
            const auto relpath = _project.relpath_to_name(shader->relative_path());
            switch (shader->type())
            {
            case gfx::shader_type::VERTEX:
                vx_shader_list << QString::fromStdString(relpath);
                break;
            case gfx::shader_type::FRAGMENT:
                fg_shader_list << QString::fromStdString(relpath);
                break;
            default:
                CRITICAL_ERROR("Unhandled shader type");
            }
        }

        auto* vxsh_combo = new QComboBox;
        vxsh_combo->addItems(vx_shader_list);

        auto* fgsh_combo = new QComboBox;
        fgsh_combo->addItems(fg_shader_list);

        auto* shaders_layout = dynamic_cast<QFormLayout*>(_ui->tab_Shaders->layout());
        shaders_layout->addRow("Vertex shader: ", vxsh_combo);
        shaders_layout->addRow("Fragment shader: ", fgsh_combo);

        connect(vxsh_combo, &QComboBox::currentTextChanged, this, [this, vxsh_combo] {
            const auto path =
                _project.name_to_abspath<project::material_asset>(_ui->itemManagerWidget->current_text().toStdString());
            auto asset = get_assets().at(path);

            const auto shader_ref = _project.name_to_abspath<project::shader_asset>(vxsh_combo->currentText().toStdString());
            asset->set_vertex_shader_ref(shader_ref);
            asset->save();
        });

        connect(fgsh_combo, &QComboBox::currentTextChanged, this, [this, fgsh_combo] {
            const auto path =
                _project.name_to_abspath<project::material_asset>(_ui->itemManagerWidget->current_text().toStdString());
            auto asset = get_assets().at(path);

            const auto shader_ref = _project.name_to_abspath<project::shader_asset>(fgsh_combo->currentText().toStdString());
            asset->set_fragment_shader_ref(shader_ref);
            asset->save();
        });
    }

    void material_manager::init_textures_tab()
    {
        if (_ui->itemManagerWidget->current_text().isEmpty())
        {
            return;
        }

        const auto path =
            _project.name_to_abspath<project::material_asset>(_ui->itemManagerWidget->current_text().toStdString());
        const auto& asset = get_assets().at(path);

        const auto matdef_path =
            _project.relpath_to_abspath<project::material_definition_asset>(asset->material_definition_ref());
        const auto& matdef_asset = _project.get_assets<project::material_definition_asset>().at(matdef_path);

        if (!_ui->tab_Textures->layout())
        {
            _ui->tab_Textures->setLayout(new QVBoxLayout);
        }

        auto* textures_layout = _ui->tab_Textures->layout();
        while (QLayoutItem* child = textures_layout->takeAt(0))
        {
            delete child->widget();
            delete child;
        }

        const auto& default_image = get_default_texture_qimage();

        for (size_t slot_index = 0; slot_index < matdef_asset->get_definition().material_texture_slot_count(); ++slot_index)
        {
            auto* twidget = new texture_slot_widget(this);

            if (asset->texture_slot_refs().size() > slot_index)
            {
                const auto& texture_ref = asset->texture_slot_refs()[slot_index];
                const auto texture_asset = _project.get_asset_by_relative_path<project::texture_asset>(texture_ref);

                const auto mip_index =
                    project::texture_asset::get_closest_sized_mip_index(80, 80, texture_asset->mip_sizes());
                const auto& [mip_w, mip_h] = texture_asset->mip_sizes()[mip_index];

                QtConcurrent::run([mip_index, texture_asset] {
                    return texture_asset->load_single_mip(mip_index);
                }).then([slot_index, mip_w, mip_h, texture_asset, twidget](std::vector<std::byte> mip) {
                    twidget->set_name(QString::fromStdString(texture_asset->relative_path()));
                    twidget->set_slot_index(static_cast<uint32_t>(slot_index));
                    twidget->set_dimensions(texture_asset->width(), texture_asset->height());
                    twidget->set_format(
                        QString::fromStdString(std::string{ magic_enum::enum_name(texture_asset->format()) }));
                    twidget->set_image(mip_to_qimage({ mip }, mip_w, mip_h, texture_asset->format()));
                });
            }
            else
            {
                twidget->set_name("__ENGINE-DEFAULT-TEXTURE__");
                twidget->set_slot_index(static_cast<uint32_t>(slot_index));
                twidget->set_dimensions(default_image.width(), default_image.height());
                twidget->set_format(
                    QString::fromStdString(std::string{ magic_enum::enum_name(engine::texture_format::R8G8B8A8_SRGB) }));
                twidget->set_image(default_image);
            }
            connect(twidget, &texture_slot_widget::clicked, this, [this, asset, slot_index] {
                auto* diag = new texture_picker_dialog(_project, this);
                diag->exec();
                if (diag->result() == QDialog::DialogCode::Accepted)
                {
                    const auto& dialog_path = diag->selected_path();
                    const auto texture_asset = _project.get_asset_by_path<project::texture_asset>(dialog_path);
                    auto texture_slot_refs = asset->texture_slot_refs();
                    if (texture_slot_refs.size() <= slot_index)
                    {
                        texture_slot_refs.resize(slot_index + 1);
                    }
                    texture_slot_refs[slot_index] = texture_asset->relative_path();
                    asset->set_texture_slot_refs(texture_slot_refs);
                    asset->save();
                    reload_material_props();
                    return;
                }
            });
            textures_layout->addWidget(twidget);
        }

        dynamic_cast<QBoxLayout*>(textures_layout)->addStretch();
    }

    void material_manager::showEvent(QShowEvent* ev)
    {
        reload_item_list();
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
            const auto path = _project.name_to_abspath<project::material_asset>(diag->name().toStdString());
            auto new_asset = std::make_shared<project::material_asset>(_project, path);
            new_asset->set_material_definition_ref(diag->matdef().toStdString());
            new_asset->mark_as_manually_loaded();
            new_asset->save();

            _project.add_asset(new_asset);
            reload_item_list();
            reload_material_props();
        }
    }

    void material_manager::slot_rename_material_clicked()
    {
        rename_asset();
        reload_material_props();
    }

    void material_manager::slot_delete_material_clicked()
    {
        delete_asset();
        reload_material_props();
    }

    void material_manager::slot_material_selection_changed()
    {
        reload_material_props();
    }
} // namespace cathedral::editor