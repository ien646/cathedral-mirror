#include <cathedral/editor/asset_managers/material_manager.hpp>

#include <cathedral/editor/asset_managers/dialogs/new_material_dialog.hpp>
#include <cathedral/editor/common/text_input_dialog.hpp>
#include <cathedral/editor/common/texture_list_widget.hpp>
#include <cathedral/editor/common/texture_picker_dialog.hpp>
#include <cathedral/editor/common/texture_slot_widget.hpp>
#include <cathedral/editor/texture_utils.hpp>

#include <cathedral/engine/default_resources.hpp>
#include <cathedral/engine/scene.hpp>
#include <cathedral/engine/texture_decompression.hpp>

#include <cathedral/core.hpp>

#include <cathedral/project/assets/material_asset.hpp>
#include <cathedral/project/project.hpp>

#include <ien/str_utils.hpp>

#include <QComboBox>
#include <QFormLayout>
#include <QLabel>
#include <QPushButton>
#include <QShowEvent>
#include <QtConcurrent/QtConcurrent>

#include <magic_enum.hpp>

#include "ui_material_manager.h"

namespace cathedral::editor
{
    material_manager::material_manager(project::project* pro, engine::scene& scene, QWidget* parent, bool allow_select)
        : QMainWindow(parent)
        , resource_manager_base(pro)
        , _ui(new Ui::material_manager)
        , _scene(scene)
        , _allow_select(allow_select)
    {
        _ui->setupUi(this);

        connect(_ui->itemManagerWidget, &item_manager::add_clicked, this, &SELF::slot_add_material_clicked);
        connect(_ui->itemManagerWidget, &item_manager::rename_clicked, this, &SELF::slot_rename_material_clicked);
        connect(_ui->itemManagerWidget, &item_manager::delete_clicked, this, &SELF::slot_delete_material_clicked);
        connect(_ui->itemManagerWidget, &item_manager::item_selection_changed, this, &SELF::slot_material_selection_changed);

        if (_allow_select)
        {
            connect(_ui->pushButton_Select, &QPushButton::clicked, this, [this] {
                emit material_selected(get_current_asset());
                close();
            });
            connect(_ui->pushButton_Cancel, &QPushButton::clicked, this, [this] { close(); });
        }
        else
        {
            delete _ui->pushButton_Cancel;
            delete _ui->pushButton_Select;
        }
    }

    item_manager* material_manager::get_item_manager_widget()
    {
        return _ui->itemManagerWidget;
    }

    const item_manager* material_manager::get_item_manager_widget() const
    {
        return _ui->itemManagerWidget;
    }

    void material_manager::reload_material_props()
    {
        init_shaders_tab();
        init_variables_tab();
        init_textures_tab();
    }

    void material_manager::init_shaders_tab()
    {
        if (_ui->tab_Shaders->layout() == nullptr)
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
        for (const auto& [path, shader] : _project->shader_assets())
        {
            switch (shader->type())
            {
            case gfx::shader_type::VERTEX:
                vx_shader_list << QString::fromStdString(shader->name());
                break;
            case gfx::shader_type::FRAGMENT:
                fg_shader_list << QString::fromStdString(shader->name());
                break;
            default:
                CRITICAL_ERROR("Unhandled shader type");
            }
        }

        if (!is_asset_selected())
        {
            return;
        }
        const auto asset = get_current_asset();

        auto* def_label = new QLabel(QString::fromStdString(asset->material_definition_ref()));

        auto* vxsh_combo = new QComboBox;
        vxsh_combo->addItem("None");
        vxsh_combo->addItems(vx_shader_list);

        auto* fgsh_combo = new QComboBox;
        fgsh_combo->addItem("None");
        fgsh_combo->addItems(fg_shader_list);

        auto* shaders_layout = dynamic_cast<QFormLayout*>(_ui->tab_Shaders->layout());
        shaders_layout->addRow("Material definition: ", def_label);
        shaders_layout->addRow("Vertex shader: ", vxsh_combo);
        shaders_layout->addRow("Fragment shader: ", fgsh_combo);

        if (!asset->vertex_shader_ref().empty())
        {
            const auto name = asset->vertex_shader_ref();
            CRITICAL_CHECK(vx_shader_list.contains(name));
            vxsh_combo->setCurrentText(QString::fromStdString(name));
        }
        if (!asset->fragment_shader_ref().empty())
        {
            const auto name = asset->fragment_shader_ref();
            CRITICAL_CHECK(fg_shader_list.contains(name));
            fgsh_combo->setCurrentText(QString::fromStdString(name));
        }

        connect(vxsh_combo, &QComboBox::currentTextChanged, this, [this, vxsh_combo] {
            const auto name = _ui->itemManagerWidget->current_text().toStdString();
            auto asset = get_assets().at(name);

            const auto shader_ref = vxsh_combo->currentText() == "None" ? "" : vxsh_combo->currentText().toStdString();

            asset->set_vertex_shader_ref(shader_ref);
            asset->save();
        });

        connect(fgsh_combo, &QComboBox::currentTextChanged, this, [this, fgsh_combo] {
            const auto name = _ui->itemManagerWidget->current_text().toStdString();
            auto asset = get_assets().at(name);

            const auto shader_ref = fgsh_combo->currentText() == "None" ? "" : fgsh_combo->currentText().toStdString();

            asset->set_fragment_shader_ref(shader_ref);
            asset->save();
        });
    }

    void material_manager::init_variables_tab()
    {
        if (_ui->tab_Variables->layout() == nullptr)
        {
            _ui->tab_Variables->setLayout(new QVBoxLayout());
        }

        auto* layout = _ui->tab_Variables->layout();
        layout->addWidget(new QLabel("Not implemented"));
    }

    void material_manager::init_textures_tab()
    {
        if (_ui->itemManagerWidget->current_text().isEmpty())
        {
            return;
        }

        const auto name = _ui->itemManagerWidget->current_text().toStdString();
        const auto& asset = get_assets().at(name);

        const auto& matdef_asset = _project->material_definition_assets().at(asset->material_definition_ref());

        if (_ui->tab_Textures->layout() == nullptr)
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
        const auto set_default_texture = [&](auto* twidget, const auto slot_index) {
            twidget->set_name("__ENGINE-DEFAULT-TEXTURE__");
            twidget->set_slot_index(static_cast<uint32_t>(slot_index));
            twidget->set_dimensions(default_image.width(), default_image.height());
            twidget->set_format(
                QString::fromStdString(std::string{ magic_enum::enum_name(engine::texture_format::R8G8B8A8_SRGB) }));
            twidget->set_image(default_image);
        };

        for (size_t slot_index = 0; slot_index < matdef_asset->get_definition().material_texture_slot_count(); ++slot_index)
        {
            auto* twidget = new texture_slot_widget(this);

            if (asset->texture_slot_refs().size() > slot_index)
            {
                const auto& texture_ref = asset->texture_slot_refs()[slot_index];
                std::shared_ptr<project::texture_asset> texture_asset;
                if (!_project->texture_assets().contains(texture_ref))
                {
                    show_error_message(QString("Asset '%1' references texture '%2', which was not found in the current "
                                               "project.\nReplacing with default texture")
                                           .arg(QString::fromStdString(asset->name()))
                                           .arg(QString::fromStdString(texture_ref)));
                    set_default_texture(twidget, slot_index);
                }
                else
                {
                    const auto texture_asset = _project->texture_assets().at(texture_ref);

                    const auto mip_index =
                        project::texture_asset::get_closest_sized_mip_index(80, 80, texture_asset->mip_sizes());
                    const auto& mip_dim = texture_asset->mip_sizes()[mip_index];

                    QtConcurrent::run([mip_index, texture_asset] { return texture_asset->load_single_mip(mip_index); })
                        .then([slot_index, mip_w = mip_dim.x, mip_h = mip_dim.y, texture_asset, twidget](
                                  std::vector<std::byte> mip) {
                            twidget->set_name(QString::fromStdString(texture_asset->name()));
                            twidget->set_slot_index(static_cast<uint32_t>(slot_index));
                            twidget->set_dimensions(texture_asset->width(), texture_asset->height());
                            twidget->set_format(
                                QString::fromStdString(std::string{ magic_enum::enum_name(texture_asset->format()) }));
                            twidget->set_image(mip_to_qimage({ mip }, mip_w, mip_h, texture_asset->format()));
                        });
                }
            }
            else
            {
                set_default_texture(twidget, slot_index);
            }
            connect(twidget, &texture_slot_widget::clicked, this, [this, asset, slot_index] {
                auto* diag = new texture_picker_dialog(*_project, this);
                diag->exec();
                if (diag->result() == QDialog::DialogCode::Accepted)
                {
                    const auto& texture_name = diag->selected_name();
                    const auto texture_asset = _project->texture_assets().at(texture_name);
                    auto texture_slot_refs = asset->texture_slot_refs();
                    if (texture_slot_refs.size() <= slot_index)
                    {
                        texture_slot_refs.resize(slot_index + 1);
                    }
                    texture_slot_refs[slot_index] = texture_asset->name();
                    asset->set_texture_slot_refs(texture_slot_refs);
                    asset->save();

                    auto& renderer = _scene.get_renderer();

                    renderer.vkctx().device().waitIdle();

                    // Update renderer material texture
                    if (!renderer.textures().contains(texture_name))
                    {
                        engine::texture_args_from_data args;
                        args.name = texture_asset->name();
                        args.format = texture_asset->format();
                        args.image_aspect_flags = vk::ImageAspectFlagBits::eColor;
                        args.sampler_info = texture_asset->sampler_info();
                        args.size = texture_asset->mip_sizes()[0];
                        args.mips = texture_asset->load_mips();

                        std::ignore = renderer.create_color_texture_from_data(args);
                    }

                    const auto& material_name = asset->name();

                    if (renderer.materials().contains(material_name))
                    {
                        renderer.materials()
                            .at(material_name)
                            ->bind_material_texture_slot(renderer.textures().at(texture_name), slot_index);
                    }

                    reload_material_props();
                    return;
                }
            });
            textures_layout->addWidget(twidget);
        }

        dynamic_cast<QBoxLayout*>(textures_layout)->addStretch();
    }

    void material_manager::showEvent([[maybe_unused]] QShowEvent* ev)
    {
        reload_item_list();
    }

    void material_manager::slot_add_material_clicked()
    {
        QStringList matdefs;
        for (const auto& [path, asset] : _project->get_assets<project::material_definition_asset>())
        {
            matdefs << QString::fromStdString(asset->name());
        }

        auto* diag = new new_material_dialog(_ui->itemManagerWidget->get_texts(), matdefs, this);
        if (diag->exec() == QDialog::DialogCode::Accepted)
        {
            const auto path = _project->name_to_abspath<project::material_asset>(diag->name().toStdString());
            auto new_asset = std::make_shared<project::material_asset>(_project, path);
            new_asset->set_material_definition_ref(diag->matdef().toStdString());
            new_asset->mark_as_manually_loaded();
            new_asset->save();

            _project->add_asset(new_asset);
            reload_item_list();

            const auto select_ok = _ui->itemManagerWidget->select_item(diag->name());
            CRITICAL_CHECK(select_ok);
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

    void material_manager::slot_material_selection_changed(std::optional<QString> selected)
    {
        if (_allow_select)
        {
            _ui->pushButton_Select->setEnabled(selected.has_value() && !selected->isEmpty());
        }
        reload_material_props();
    }
} // namespace cathedral::editor