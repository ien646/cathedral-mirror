#include <cathedral/editor/asset_managers/material_definition_manager.hpp>

#include <cathedral/editor/common/message.hpp>
#include <cathedral/editor/common/text_input_dialog.hpp>

#include <cathedral/project/project.hpp>

#include <ien/str_utils.hpp>

#include <magic_enum.hpp>

#include <QComboBox>

#include <filesystem>

#include "ui_material_definition_manager.h"

namespace fs = std::filesystem;

namespace cathedral::editor
{
    material_definition_manager::material_definition_manager(project::project& pro, QWidget* parent)
        : QMainWindow(parent)
        , _project(pro)
        , _ui(new Ui::material_definition_manager())
    {
        _ui->setupUi(this);
        
        connect(_ui->actionClose, &QAction::triggered, this, &QMainWindow::close);

        connect(_ui->pushButton_CustomTypes, &QPushButton::clicked, this, [this] {
            show_error_message("Not implemented", this);
        });

        connect(
            _ui->listWidget_Materials,
            &QListWidget::itemSelectionChanged,
            this,
            &material_definition_manager::slot_selected_changed);
        connect(_ui->pushButton_Add, &QPushButton::clicked, this, &material_definition_manager::slot_new_material_definition);
        connect(_ui->pushButton_Save, &QPushButton::clicked, this, &material_definition_manager::slot_save_clicked);

        reload();
    }

    void material_definition_manager::reload()
    {
        _ui->listWidget_Materials->clear();

        for (auto [path, asset] : _project.material_definition_assets())
        {
            const auto relative_path = ien::str_trim(ien::str_split(path, _project.material_definitions_path())[0], '/');
            const auto name = std::filesystem::path(relative_path).replace_extension().string();

            _ui->listWidget_Materials->addItem(QString::fromStdString(name));

            const auto& def = asset->get_definition();
            _ui->spinBox_MatTexSlots->setValue(def.material_texture_slot_count());
            _ui->spinBox_NodeTexSlots->setValue(def.node_texture_slot_count());

            reload_variables(*asset);
        }

        _ui->listWidget_Materials->sortItems(Qt::SortOrder::AscendingOrder);
    }

    void material_definition_manager::reload_variables(const project::material_definition_asset& asset)
    {
        _ui->tableWidget_MaterialVariables->clearContents();
        _ui->tableWidget_NodeVariables->clearContents();

        QStringList binding_list;
        for (const auto& b : magic_enum::enum_names<engine::material_uniform_binding>())
        {
            binding_list << QString::fromStdString(std::string{ b });
        }

        const auto& def = asset.get_definition();

        const std::map<uint32_t, engine::material_definition::variable>
            sorted_material_variables(def.material_variables().begin(), def.material_variables().end());

        const std::map<uint32_t, engine::material_definition::variable>
            sorted_node_variables(def.node_variables().begin(), def.node_variables().end());

        uint32_t current_offset = 0;
        for (const auto& [index, var] : sorted_material_variables)
        {
            auto* id_label = new QLabel(QString::number(index));
            auto* name_label = new QLabel(QString::fromStdString(var.name));
            auto* type_label = new QLabel(QString::fromStdString(std::string{ magic_enum::enum_name(var.type) }));
            auto* offset_label = new QLabel(QString::number(current_offset));
            auto* binding_combo = new QComboBox;
            binding_combo->addItems(binding_list);

            _ui->tableWidget_MaterialVariables->setCellWidget(index, 0, id_label);
            _ui->tableWidget_MaterialVariables->setCellWidget(index, 1, name_label);
            _ui->tableWidget_MaterialVariables->setCellWidget(index, 2, type_label);
            _ui->tableWidget_MaterialVariables->setCellWidget(index, 3, offset_label);
            _ui->tableWidget_MaterialVariables->setCellWidget(index, 4, binding_combo);

            current_offset += gfx::shader_data_type_offset(var.type);
        }

        current_offset = 0;
        for (const auto& [index, var] : sorted_node_variables)
        {
            auto* id_label = new QLabel(QString::number(index));
            auto* name_label = new QLabel(QString::fromStdString(var.name));
            auto* type_label = new QLabel(QString::fromStdString(std::string{ magic_enum::enum_name(var.type) }));
            auto* offset_label = new QLabel(QString::number(current_offset));
            auto* binding_combo = new QComboBox;
            binding_combo->addItems(binding_list);

            _ui->tableWidget_NodeVariables->setCellWidget(index, 0, id_label);
            _ui->tableWidget_NodeVariables->setCellWidget(index, 1, name_label);
            _ui->tableWidget_NodeVariables->setCellWidget(index, 2, type_label);
            _ui->tableWidget_NodeVariables->setCellWidget(index, 3, offset_label);
            _ui->tableWidget_NodeVariables->setCellWidget(index, 4, binding_combo);

            current_offset += gfx::shader_data_type_offset(var.type);
        }
    }

    std::shared_ptr<project::material_definition_asset> material_definition_manager::get_current_asset()
    {
        CRITICAL_CHECK(!_ui->listWidget_Materials->selectedItems().empty());
        const auto selected_text = _ui->listWidget_Materials->selectedItems()[0]->text() + ".casset";
        const auto path = fs::path(_project.material_definitions_path()) / selected_text.toStdString();
        return _project.get_asset_by_path<project::material_definition_asset>(path);
    }

    void material_definition_manager::slot_selected_changed()
    {
        if (_ui->listWidget_Materials->selectedItems().empty())
        {
            _ui->pushButton_Save->setEnabled(false);
            return;
        }

        auto asset = get_current_asset();
        if (!asset->is_loaded())
        {
            asset->load();
        }

        const auto& def = asset->get_definition();
        _ui->spinBox_MatTexSlots->setValue(def.material_texture_slot_count());
        _ui->spinBox_NodeTexSlots->setValue(def.node_texture_slot_count());

        reload_variables(*asset);

        _ui->pushButton_Save->setEnabled(true);
    }

    void material_definition_manager::slot_new_material_definition()
    {
        auto* diag = new text_input_dialog(this, "Create new shader", "Name:", false, "new_shader");
        if (diag->exec() == QDialog::Accepted)
        {
            const auto path = (fs::path(_project.materials_path()) / diag->result().toStdString()).string() + ".casset";

            auto new_asset = std::make_shared<project::material_definition_asset>(_project, path);
            new_asset->set_definition({});
            new_asset->mark_as_manually_loaded();
            new_asset->save();

            _project.add_asset(new_asset);
        }
    }

    void material_definition_manager::slot_save_clicked()
    {
        auto asset = get_current_asset();

        const auto mat_slots = _ui->spinBox_MatTexSlots->value();
        const auto node_slots = _ui->spinBox_NodeTexSlots->value();

        engine::material_definition new_def;
        new_def.set_material_texture_slot_count(mat_slots);
        new_def.set_node_texture_slot_count(node_slots);

        for (const auto& [index, var] : asset->get_definition().material_variables())
        {
            new_def.set_material_variable(index, var);
        }

        for (const auto& [index, var] : asset->get_definition().node_variables())
        {
            new_def.set_node_variable(index, var);
        }

        asset->set_definition(std::move(new_def));
        asset->save();
    }
} // namespace cathedral::editor