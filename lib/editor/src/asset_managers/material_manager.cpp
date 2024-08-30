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
#include <QtConcurrent>

#include <magic_enum.hpp>

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

        connect(_ui->itemManagerWidget, &item_manager::add_clicked, this, &SELF::slot_add_material_clicked);
        connect(_ui->itemManagerWidget, &item_manager::rename_clicked, this, &SELF::slot_rename_material_clicked);
        connect(_ui->itemManagerWidget, &item_manager::delete_clicked, this, &SELF::slot_delete_material_clicked);
        connect(_ui->itemManagerWidget, &item_manager::item_selection_changed, this, &SELF::slot_material_selection_changed);

        reload_item_list();
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
                break;
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

    void material_manager::init_textures_tab()
    {
        if (!_ui->itemManagerWidget->current_text())
        {
            return;
        }

        const auto path =
            (std::filesystem::path(get_assets_path()) / _ui->itemManagerWidget->current_text()->toStdString()).string() +
            ".casset";
        const auto& asset = get_assets().at(path);

        const auto matdef_path =
            (std::filesystem::path(_project.material_definitions_path()) / asset->material_definition_ref()).string();
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

                twidget->set_name(QString::fromStdString(texture_asset->relative_path()));
                twidget->set_slot_index(slot_index);
                twidget->set_dimensions(texture_asset->width(), texture_asset->height());
                twidget->set_format(QString::fromStdString(std::string{ magic_enum::enum_name(texture_asset->format()) }));

                QtConcurrent::run([texture_asset] {
                    return texture_asset->load_mips();
                }).then([texture_asset, twidget](std::vector<std::vector<uint8_t>> mips) {
                    const auto adequate_mip_index =
                        project::texture_asset::get_closest_sized_mip_index(80, 80, texture_asset->mip_sizes());
                    const auto [mip_width, mip_height] = texture_asset->mip_sizes()[adequate_mip_index];

                    std::vector<uint8_t> image_data = [&] -> std::vector<uint8_t> {
                        if (engine::is_compressed_format(texture_asset->format()))
                        {
                            return engine::decompress_texture_data(
                                mips[adequate_mip_index].data(),
                                mips[adequate_mip_index].size(),
                                mip_width,
                                mip_height,
                                engine::get_format_compression_type(texture_asset->format()));
                        }
                        else
                        {
                            return mips[adequate_mip_index];
                        }
                    }();

                    const auto rgba_data = image_data_to_qrgba(image_data, texture_asset->format());

                    QImage image(rgba_data.data(), mip_width, mip_height, QImage::Format::Format_RGBA8888);
                    twidget->set_image(image);

                    texture_asset->unload();
                });
            }
            else
            {
                twidget->set_name("__ENGINE-DEFAULT-TEXTURE__");
                twidget->set_slot_index(slot_index);
                twidget->set_dimensions(default_image.width(), default_image.height());
                twidget->set_format(QString::fromStdString(
                    std::string{ magic_enum::enum_name(engine::texture_format::R8G8B8A8_SRGB) }));
                twidget->set_image(default_image);
            }
            connect(twidget, &texture_slot_widget::clicked, this, [this, asset, slot_index] {
                auto* diag = new texture_picker_dialog(_project, this);
                diag->exec();
                if (diag->result() == QDialog::DialogCode::Accepted)
                {
                    const auto path = diag->selected_path();
                    const auto texture_asset = _project.get_asset_by_path<project::texture_asset>(path);
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