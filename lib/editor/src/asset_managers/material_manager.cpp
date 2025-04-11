#include <cathedral/editor/asset_managers/material_manager.hpp>

#include <cathedral/editor/asset_managers/dialogs/new_material_dialog.hpp>
#include <cathedral/editor/common/text_input_dialog.hpp>
#include <cathedral/editor/common/texture_list_widget.hpp>
#include <cathedral/editor/common/texture_picker_dialog.hpp>
#include <cathedral/editor/common/texture_slot_widget.hpp>
#include <cathedral/editor/texture_utils.hpp>
#include <cathedral/editor/utils.hpp>

#include <cathedral/engine/default_resources.hpp>
#include <cathedral/engine/scene.hpp>
#include <cathedral/engine/texture_decompression.hpp>

#include <cathedral/core.hpp>

#include <cathedral/project/assets/material_asset.hpp>
#include <cathedral/project/project.hpp>

#include <ien/str_utils.hpp>

#include <QComboBox>
#include <QFormLayout>
#include <QHeaderView>
#include <QLabel>
#include <QPushButton>
#include <QShowEvent>
#include <QTableWidget>
#include <QTableWidgetItem>
#include <QtConcurrent/QtConcurrent>

#include <magic_enum.hpp>
#include <utility>

#include "ui_material_manager.h"

namespace cathedral::editor
{
    material_manager::material_manager(
        project::project* pro,
        std::shared_ptr<engine::scene> scene,
        QWidget* parent,
        bool allow_select)
        : QMainWindow(parent)
        , resource_manager_base(pro)
        , _ui(new Ui::material_manager)
        , _scene(std::move(scene))
        , _allow_select(allow_select)
    {
        _ui->setupUi(this);

        connect(_ui->itemManagerWidget, &item_manager::add_clicked, this, &SELF::handle_add_material_clicked);
        connect(_ui->itemManagerWidget, &item_manager::rename_clicked, this, &SELF::handle_rename_material_clicked);
        connect(_ui->itemManagerWidget, &item_manager::delete_clicked, this, &SELF::handle_delete_material_clicked);
        connect(_ui->itemManagerWidget, &item_manager::item_selection_changed, this, &SELF::handle_material_selection_changed);

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

        auto* vxsh_combo = new QComboBox;
        vxsh_combo->addItem("None");
        vxsh_combo->addItems(vx_shader_list);

        auto* fgsh_combo = new QComboBox;
        fgsh_combo->addItem("None");
        fgsh_combo->addItems(fg_shader_list);

        QStringList domain_list;
        for (const auto& name : magic_enum::enum_names<engine::material_domain>())
        {
            domain_list << QSTR(name);
        }
        auto* domain_combo = new QComboBox;
        domain_combo->addItems(domain_list);

        auto* shaders_layout = dynamic_cast<QFormLayout*>(_ui->tab_Shaders->layout());
        shaders_layout->addRow("Vertex shader: ", vxsh_combo);
        shaders_layout->addRow("Fragment shader: ", fgsh_combo);
        shaders_layout->addRow("Domain: ", domain_combo);

        if (!asset->vertex_shader_ref().empty())
        {
            const auto name = asset->vertex_shader_ref();
            if (!vx_shader_list.contains(name))
            {
                asset->set_vertex_shader_ref({});
                asset->save();
            }
            else
            {
                vxsh_combo->setCurrentText(QString::fromStdString(name));
            }
        }
        if (!asset->fragment_shader_ref().empty())
        {
            const auto name = asset->fragment_shader_ref();
            if (!fg_shader_list.contains(name))
            {
                asset->set_fragment_shader_ref({});
                asset->save();
            }
            else
            {
                fgsh_combo->setCurrentText(QString::fromStdString(name));
            }
        }

        connect(vxsh_combo, &QComboBox::currentTextChanged, this, [this, fgsh_combo, vxsh_combo] {
            auto asset = get_current_asset();

            const auto shader_ref = vxsh_combo->currentText() == "None" ? "" : vxsh_combo->currentText().toStdString();

            asset->set_vertex_shader_ref(shader_ref);
            asset->save();

            if (fgsh_combo->currentText() == "None" || vxsh_combo->currentText() == "None")
            {
                _scene->get_renderer().materials().erase(asset->name());
            }

            init_variables_tab();
        });

        connect(fgsh_combo, &QComboBox::currentTextChanged, this, [this, fgsh_combo, vxsh_combo] {
            auto asset = get_current_asset();

            const auto shader_ref = fgsh_combo->currentText() == "None" ? "" : fgsh_combo->currentText().toStdString();

            asset->set_fragment_shader_ref(shader_ref);
            asset->save();

            if (fgsh_combo->currentText() == "None" || vxsh_combo->currentText() == "None")
            {
                _scene->get_renderer().materials().erase(asset->name());
            }

            init_variables_tab();
        });

        connect(domain_combo, &QComboBox::currentTextChanged, this, [this, domain_combo] {
            auto asset = get_current_asset();

            const auto enum_value_opt =
                magic_enum::enum_cast<engine::material_domain>(domain_combo->currentText().toStdString());
            CRITICAL_CHECK(enum_value_opt.has_value(), "Invalid material domain");

            asset->set_domain(*enum_value_opt);
            asset->save();
        });
    }

    void material_manager::init_variables_tab()
    {
        if (_ui->tab_Variables->layout() == nullptr)
        {
            _ui->tab_Variables->setLayout(new QVBoxLayout());
        }

        QLayoutItem* child = nullptr;
        while ((child = _ui->tab_Variables->layout()->takeAt(0)) != nullptr)
        {
            _ui->tab_Variables->layout()->removeWidget(child->widget());
            delete child->widget();
            delete child;
        }

        auto* layout = _ui->tab_Variables->layout();

        auto asset = get_current_asset();
        auto material = _scene->load_material(asset->name());

        if (material.expired())
        {
            layout->addWidget(new QLabel("Shaders must be loaded before editing variables"));
            return;
        }

        QStringList mat_uniform_bindings = { "None" };
        for (const auto& name : magic_enum::enum_names<engine::shader_material_uniform_binding>())
        {
            mat_uniform_bindings << QSTR(name);
        }

        QStringList node_uniform_bindings = { "None" };
        for (const auto& name : magic_enum::enum_names<engine::shader_node_uniform_binding>())
        {
            node_uniform_bindings << QSTR(name);
        }

        QStringList texture_bindings = { "None" };
        for (const auto& name : magic_enum::enum_names<engine::shader_material_texture_binding>())
        {
            texture_bindings << QSTR(name);
        }

        auto* matvars_table_widget = new QTableWidget;
        auto* nodevars_table_widget = new QTableWidget;
        auto* mattex_table_widget = new QTableWidget;

        layout->addWidget(new QLabel("Material variables"));
        layout->addWidget(matvars_table_widget);
        layout->addWidget(new QLabel("Node variables"));
        layout->addWidget(nodevars_table_widget);
        layout->addWidget(new QLabel("Material textures"));
        layout->addWidget(mattex_table_widget);

        matvars_table_widget->setColumnCount(5);
        nodevars_table_widget->setColumnCount(5);
        mattex_table_widget->setColumnCount(3);

        mattex_table_widget->horizontalHeader()->setSectionResizeMode(QHeaderView::ResizeMode::ResizeToContents);
        nodevars_table_widget->horizontalHeader()->setSectionResizeMode(QHeaderView::ResizeMode::ResizeToContents);
        mattex_table_widget->horizontalHeader()->setSectionResizeMode(QHeaderView::ResizeMode::ResizeToContents);

        mattex_table_widget->horizontalHeader()->setSectionResizeMode(0, QHeaderView::ResizeMode::Stretch);
        nodevars_table_widget->horizontalHeader()->setSectionResizeMode(0, QHeaderView::ResizeMode::Stretch);
        mattex_table_widget->horizontalHeader()->setSectionResizeMode(0, QHeaderView::ResizeMode::Stretch);

        mattex_table_widget->verticalHeader()->setVisible(false);
        nodevars_table_widget->verticalHeader()->setVisible(false);
        mattex_table_widget->verticalHeader()->setVisible(false);

        matvars_table_widget->setHorizontalHeaderLabels(QStringList{ "Name", "Type", "Count", "Offset", "Binding" });
        nodevars_table_widget->setHorizontalHeaderLabels(QStringList{ "Name", "Type", "Count", "Offset", "Binding" });
        mattex_table_widget->setHorizontalHeaderLabels(QStringList{ "Name", "Count", "Binding" });

        const auto number_label = [](const auto number) -> QWidget* {
            auto* result = new QLabel(QString::number(number));
            result->setAlignment(Qt::AlignmentFlag::AlignRight | Qt::AlignmentFlag::AlignVCenter);
            result->setStyleSheet("QLabel{ padding-right: 2px; }");
            return result;
        };

        uint32_t offset = 0;
        for (size_t i = 0; i < material.lock()->material_variables().size(); ++i)
        {
            const auto& var = material.lock()->material_variables()[i];
            const auto& bindings = material.lock()->material_bindings();

            auto* bindings_combo = new QComboBox(this);
            bindings_combo->addItems(mat_uniform_bindings);

            auto it = std::ranges::find_if(bindings, [&](const auto& kvp) { return kvp.second == var.name; });
            if (it != bindings.end())
            {
                bindings_combo->setCurrentText(QSTR(magic_enum::enum_name(it->first)));
            }

            connect(
                bindings_combo,
                &QComboBox::currentTextChanged,
                this,
                [asset, material, name = var.name](const QString& text) {
                    const auto value_opt =
                        magic_enum::enum_cast<engine::shader_material_uniform_binding>(text.toStdString());
                    material.lock()->set_material_binding_for_var(name, value_opt);
                    asset->set_material_variable_binding(name, value_opt);
                    asset->save();
                });

            matvars_table_widget->insertRow(i);

            matvars_table_widget->setCellWidget(i, 0, new QLabel(QSTR(var.name)));
            matvars_table_widget->setCellWidget(i, 1, new QLabel(QSTR(magic_enum::enum_name(var.type))));
            matvars_table_widget->setCellWidget(i, 2, number_label(var.count));
            matvars_table_widget->setCellWidget(i, 3, number_label(offset));
            matvars_table_widget->setCellWidget(i, 4, bindings_combo);
            offset = gfx::shader_data_type_offset(var.type, var.count, 0);
        }

        offset = 0;
        for (size_t i = 0; i < material.lock()->node_variables().size(); ++i)
        {
            const auto& var = material.lock()->node_variables()[i];
            const auto& bindings = material.lock()->node_bindings();

            auto* bindings_combo = new QComboBox;
            bindings_combo->addItems(node_uniform_bindings);

            auto it = std::ranges::find_if(bindings, [&](const auto& kvp) { return kvp.second == var.name; });
            if (it != bindings.end())
            {
                bindings_combo->setCurrentText(QSTR(magic_enum::enum_name(it->first)));
            }

            connect(
                bindings_combo,
                &QComboBox::currentTextChanged,
                this,
                [asset, material, name = var.name](const QString& text) {
                    const auto value_opt = magic_enum::enum_cast<engine::shader_node_uniform_binding>(text.toStdString());
                    material.lock()->set_node_binding_for_var(name, value_opt);
                    asset->set_node_variable_binding(name, value_opt);
                    asset->save();
                });

            nodevars_table_widget->insertRow(i);

            nodevars_table_widget->setCellWidget(i, 0, new QLabel(QSTR(var.name)));
            nodevars_table_widget->setCellWidget(i, 1, new QLabel(QSTR(magic_enum::enum_name(var.type))));
            nodevars_table_widget->setCellWidget(i, 2, number_label(var.count));
            nodevars_table_widget->setCellWidget(i, 3, number_label(offset));
            nodevars_table_widget->setCellWidget(i, 4, bindings_combo);
            offset = gfx::shader_data_type_offset(var.type, var.count, 0);
        }

        for (size_t i = 0; i < material.lock()->material_texture_slots(); ++i)
        {
            auto* bindings_combo = new QComboBox(this);
            bindings_combo->addItems(texture_bindings);

            mattex_table_widget->insertRow(i);

            const auto& name = material.lock()->material_texture_names()[i];
            mattex_table_widget->setCellWidget(i, 0, new QLabel(QSTR(name)));
            mattex_table_widget->setCellWidget(i, 1, number_label(1));
            mattex_table_widget->setCellWidget(i, 2, bindings_combo);
        }
    }

    void material_manager::init_textures_tab()
    {
        if (_ui->itemManagerWidget->current_text().isEmpty())
        {
            return;
        }

        const auto& asset = get_current_asset();

        const auto material = _scene->load_material(asset->name());
        if (material.expired())
        {
            return;
        }

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
            twidget->set_format(QSTR(magic_enum::enum_name(engine::texture_format::R8G8B8A8_SRGB)));
            twidget->set_image(default_image);
        };

        for (size_t slot_index = 0; slot_index < material.lock()->material_texture_slots(); ++slot_index)
        {
            auto* twidget = new texture_slot_widget(this);

            if (asset->texture_slot_refs().size() > slot_index)
            {
                const auto& texture_ref = asset->texture_slot_refs()[slot_index];
                std::shared_ptr<project::texture_asset> texture_asset;
                if (!_project->texture_assets().contains(texture_ref))
                {
                    show_error_message(QSTR(
                        "Asset '{}' references texture '{}', which was not found in the current "
                        "project.\nReplacing with default texture",
                        asset->name(),
                        texture_ref));
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
            connect(twidget, &texture_slot_widget::clicked, this, [this, slot_index] {
                handle_texture_slot_clicked(slot_index);
            });
            textures_layout->addWidget(twidget);
        }

        dynamic_cast<QBoxLayout*>(textures_layout)->addStretch();
    }

    void material_manager::showEvent([[maybe_unused]] QShowEvent* ev)
    {
        reload_item_list();
    }

    void material_manager::handle_add_material_clicked()
    {
        auto* diag = new new_material_dialog(_ui->itemManagerWidget->get_texts(), this);
        if (diag->exec() == QDialog::DialogCode::Accepted)
        {
            const auto path = _project->name_to_abspath<project::material_asset>(diag->name().toStdString());
            auto new_asset = std::make_shared<project::material_asset>(_project, path);
            new_asset->set_domain(engine::material_domain::OPAQUE);
            new_asset->mark_as_manually_loaded();
            new_asset->save();

            _project->add_asset(new_asset);
            reload_item_list();

            const auto select_ok = _ui->itemManagerWidget->select_item(diag->name());
            CRITICAL_CHECK(select_ok, "Failure selecting item");
        }
    }

    void material_manager::handle_rename_material_clicked()
    {
        rename_asset();
        reload_material_props();
    }

    void material_manager::handle_delete_material_clicked()
    {
        delete_asset();
        reload_material_props();
    }

    void material_manager::handle_material_selection_changed(std::optional<QString> selected)
    {
        if (_allow_select)
        {
            _ui->pushButton_Select->setEnabled(selected.has_value() && !selected->isEmpty());
        }
        reload_material_props();
    }

    void material_manager::handle_texture_slot_clicked(uint32_t slot)
    {
        const auto& asset = get_current_asset();

        auto* diag = new texture_picker_dialog(*_project, this);
        diag->exec();
        if (diag->result() == QDialog::DialogCode::Accepted)
        {
            const auto& texture_name = diag->selected_name();
            const auto texture_asset = _project->texture_assets().at(texture_name);
            auto texture_slot_refs = asset->texture_slot_refs();
            if (texture_slot_refs.size() <= slot)
            {
                texture_slot_refs.resize(slot + 1);
            }
            texture_slot_refs[slot] = texture_asset->name();
            asset->set_texture_slot_refs(texture_slot_refs);
            asset->save();

            auto& renderer = _scene->get_renderer();

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
                renderer.materials().at(material_name)->bind_material_texture_slot(renderer.textures().at(texture_name), slot);
            }

            reload_material_props();
            return;
        }
    }

    void material_manager::handle_material_props_changed()
    {
        const auto asset = get_current_asset();

        auto& renderer = _scene->get_renderer();
        if (renderer.materials().contains(asset->name()))
        {
            renderer.materials().erase(asset->name());
            if (!asset->vertex_shader_ref().empty() && !asset->fragment_shader_ref().empty())
            {
                const auto vx_shader_asset = _project->get_asset_by_name<project::shader_asset>(asset->vertex_shader_ref());
                const auto fg_shader_asset =
                    _project->get_asset_by_name<project::shader_asset>(asset->fragment_shader_ref());

                engine::material_args args;
                args.name = asset->name();
                args.domain = asset->domain();
                args.vertex_shader_source = vx_shader_asset->source();
                args.fragment_shader_source = fg_shader_asset->source();
                args.material_bindings = asset->material_variable_bindings();
                args.node_bindings = asset->node_variable_bindings();
                std::ignore = renderer.create_material(args);
            }
        }
    }
} // namespace cathedral::editor