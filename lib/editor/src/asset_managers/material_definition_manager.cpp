#include <cathedral/editor/asset_managers/material_definition_manager.hpp>

#include <cathedral/editor/common/message.hpp>
#include <cathedral/editor/common/text_input_dialog.hpp>
#include <cathedral/editor/common/text_output_dialog.hpp>

#include <cathedral/project/project.hpp>

#include <ien/str_utils.hpp>

#include <magic_enum.hpp>

#include <QComboBox>
#include <QHBoxLayout>
#include <QLineEdit>
#include <QPushButton>
#include <QTableWidget>

#include <filesystem>

#include "ui_material_definition_manager.h"

namespace fs = std::filesystem;

namespace cathedral::editor
{
    material_definition_manager::material_definition_manager(project::project& pro, QWidget* parent)
        : QMainWindow(parent)
        , resource_manager_base(pro)
        , _ui(new Ui::material_definition_manager())
    {
        _ui->setupUi(this);

        connect(_ui->actionClose, &QAction::triggered, this, &QMainWindow::close);

        connect(_ui->itemManagerWidget, &item_manager::item_selection_changed, this, &SELF::slot_selected_changed);

        connect(_ui->itemManagerWidget, &item_manager::add_clicked, this, &SELF::slot_add_definition_clicked);
        connect(_ui->itemManagerWidget, &item_manager::rename_clicked, this, &SELF::slot_rename_definition_clicked);
        connect(_ui->itemManagerWidget, &item_manager::delete_clicked, this, &SELF::slot_delete_definition_clicked);

        connect(_ui->pushButton_MatAddVar, &QPushButton::clicked, this, &SELF::slot_add_material_variable_clicked);
        connect(_ui->pushButton_NodeAddVar, &QPushButton::clicked, this, &SELF::slot_add_node_variable_clicked);

        connect(_ui->pushButton_CustomTypes, &QPushButton::clicked, this, &SELF::slot_custom_types_clicked);
        connect(_ui->pushButton_Save, &QPushButton::clicked, this, &SELF::slot_save_clicked);

        connect(_ui->pushButton_MatGlslStruct, &QPushButton::clicked, this, &SELF::slot_mat_glsl_struct_clicked);
        connect(_ui->pushButton_NodeGlslStruct, &QPushButton::clicked, this, &SELF::slot_node_glsl_struct_clicked);

        connect(_ui->pushButton_MatCppStruct, &QPushButton::clicked, this, &SELF::slot_mat_cpp_struct_clicked);
        connect(_ui->pushButton_NodeCppStruct, &QPushButton::clicked, this, &SELF::slot_node_cpp_struct_clicked);

        reload();
    }

    item_manager* material_definition_manager::get_item_manager_widget()
    {
        return _ui->itemManagerWidget;
    }

    void material_definition_manager::reload_variables()
    {
        _ui->tableWidget_MaterialVariables->clearContents();
        _ui->tableWidget_NodeVariables->clearContents();

        _ui->tableWidget_MaterialVariables->setRowCount(_material_variables.size());
        _ui->tableWidget_NodeVariables->setRowCount(_node_variables.size());

        for (size_t i = 0; i < _material_variables.size(); ++i)
        {
            set_row_for_material_variable(i);
        }

        for (size_t i = 0; i < _node_variables.size(); ++i)
        {
            set_row_for_node_variable(i);
        }
    }

    std::shared_ptr<project::material_definition_asset> material_definition_manager::get_current_asset()
    {
        CRITICAL_CHECK(_ui->itemManagerWidget->current_item().has_value());
        const auto selected_text = *_ui->itemManagerWidget->current_text() + ".casset";
        const auto path = fs::path(_project.material_definitions_path()) / selected_text.toStdString();
        return _project.get_asset_by_path<project::material_definition_asset>(path.string());
    }

    void material_definition_manager::
        set_row_for_variable(uint32_t row_index, QTableWidget* table_widget, std::vector<engine::shader_variable>& variables)
    {
        QStringList binding_list;
        binding_list << "NONE";
        for (const auto& b : magic_enum::enum_names<engine::shader_uniform_binding>())
        {
            binding_list << QString::fromStdString(std::string{ b });
        }

        QStringList shader_types_list;
        for (const auto& name : magic_enum::enum_names<gfx::shader_data_type>())
        {
            shader_types_list << QString::fromStdString(std::string{ name });
        }

        uint32_t offset = 0;
        for (size_t i = 0; i < row_index; ++i)
        {
            offset += gfx::shader_data_type_offset(variables[i].type) * variables[i].count;
        }

        const auto& var = variables[row_index];
        auto* id_label = new QLabel(QString::number(row_index));
        auto* name_edit = new QLineEdit(QString::fromStdString(var.name));

        auto* type_combo = new QComboBox;
        type_combo->addItems(shader_types_list);

        auto* count_spinbox = new QSpinBox;
        count_spinbox->setMinimum(1);
        count_spinbox->setMaximum(1024);
        count_spinbox->setValue(var.count);

        auto* offset_label = new QLabel(QString::number(offset));

        auto* binding_combo = new QComboBox;
        binding_combo->addItems(binding_list);

        auto* controls_widget = new QWidget;
        auto* controls_layout = new QHBoxLayout;
        auto* button_remove_var = new QPushButton("-");
        auto* button_move_up_var = new QPushButton("^");
        auto* button_move_down_var = new QPushButton("v");
        controls_layout->addWidget(button_remove_var);
        controls_layout->addWidget(button_move_up_var);
        controls_layout->addWidget(button_move_down_var);
        controls_layout->setContentsMargins(4, 0, 4, 0);
        controls_widget->setLayout(controls_layout);

        table_widget->setCellWidget(row_index, 0, id_label);
        table_widget->setCellWidget(row_index, 1, name_edit);
        table_widget->setCellWidget(row_index, 2, type_combo);
        table_widget->setCellWidget(row_index, 3, count_spinbox);
        table_widget->setCellWidget(row_index, 4, offset_label);
        table_widget->setCellWidget(row_index, 5, binding_combo);
        table_widget->setCellWidget(row_index, 6, controls_widget);

        table_widget->horizontalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);

        type_combo->setCurrentText(QString::fromStdString(std::string{ magic_enum::enum_name(var.type) }));
        if (var.binding)
        {
            binding_combo->setCurrentText(QString::fromStdString(std::string{ magic_enum::enum_name(*var.binding) }));
        }

        connect(name_edit, &QLineEdit::editingFinished, this, [&variables, name_edit, row_index, this] {
            variables[row_index].name = name_edit->text().toStdString();
            reload_variables();
        });

        connect(type_combo, &QComboBox::currentTextChanged, this, [&variables, row_index, this](const QString& text) {
            auto type_opt = magic_enum::enum_cast<gfx::shader_data_type>(text.toStdString());
            if (type_opt)
            {
                variables[row_index].type = *type_opt;
            }
            reload_variables();
        });

        connect(count_spinbox, &QSpinBox::valueChanged, this, [&variables, row_index, this](int value) {
            variables[row_index].count = value;
            reload_variables();
        });

        connect(binding_combo, &QComboBox::currentTextChanged, this, [&variables, row_index, this](const QString& text) {
            auto binding_opt = magic_enum::enum_cast<engine::shader_uniform_binding>(text.toStdString());
            variables[row_index].binding = binding_opt;
            reload_variables();
        });

        connect(button_remove_var, &QPushButton::clicked, this, [&variables, row_index, this] {
            variables.erase(variables.begin() + row_index);
            reload_variables();
        });

        connect(button_move_up_var, &QPushButton::clicked, this, [&variables, row_index, this] {
            if (row_index == 0 || variables.size() == 1)
            {
                return;
            }
            std::swap(variables[row_index], variables[row_index - 1]);
            reload_variables();
        });

        connect(button_move_down_var, &QPushButton::clicked, this, [&variables, row_index, this] {
            if (row_index >= variables.size() - 1 || variables.size() == 1)
            {
                return;
            }
            std::swap(variables[row_index], variables[row_index + 1]);
            reload_variables();
        });
    }

    void material_definition_manager::set_row_for_material_variable(uint32_t row_index)
    {
        set_row_for_variable(row_index, _ui->tableWidget_MaterialVariables, _material_variables);
    }

    void material_definition_manager::set_row_for_node_variable(uint32_t row_index)
    {
        set_row_for_variable(row_index, _ui->tableWidget_NodeVariables, _node_variables);
    }

    void material_definition_manager::slot_selected_changed()
    {
        const bool item_selected = _ui->itemManagerWidget->current_item().has_value();

        _ui->pushButton_MatAddVar->setEnabled(item_selected);
        _ui->pushButton_NodeAddVar->setEnabled(item_selected);
        _ui->pushButton_Save->setEnabled(item_selected);
        _ui->pushButton_MatGlslStruct->setEnabled(item_selected);
        _ui->pushButton_NodeGlslStruct->setEnabled(item_selected);
        _ui->pushButton_MatCppStruct->setEnabled(item_selected);
        _ui->pushButton_NodeCppStruct->setEnabled(item_selected);

        if (!item_selected)
        {
            return;
        }

        auto asset = get_current_asset();
        project::asset_load_guard load_guard(asset);

        const auto& def = asset->get_definition();
        _ui->spinBox_MatTexSlots->setValue(def.material_texture_slot_count());
        _ui->spinBox_NodeTexSlots->setValue(def.node_texture_slot_count());

        _material_variables = asset->get_definition().material_variables();
        _node_variables = asset->get_definition().node_variables();

        reload_variables();

        _ui->pushButton_Save->setEnabled(true);
    }

    void material_definition_manager::slot_add_definition_clicked()
    {
        auto* diag =
            new text_input_dialog(this, "Create new material definition", "Name:", false, "new_material_definition");
        if (diag->exec() == QDialog::Accepted)
        {
            const auto path =
                (fs::path(_project.material_definitions_path()) / diag->result().toStdString()).string() + ".casset";

            auto new_asset = std::make_shared<project::material_definition_asset>(_project, path);
            new_asset->set_definition({});
            new_asset->mark_as_manually_loaded();
            new_asset->save();

            _project.add_asset(new_asset);
            reload();
        }
    }

    void material_definition_manager::slot_rename_definition_clicked()
    {
        rename_asset();
    }

    void material_definition_manager::slot_delete_definition_clicked()
    {
        delete_asset();
    }

    void material_definition_manager::slot_add_material_variable_clicked()
    {
        if (!_ui->itemManagerWidget->current_item())
        {
            return;
        }

        _material_variables.emplace_back();
        reload_variables();
    }

    void material_definition_manager::slot_add_node_variable_clicked()
    {
        if (!_ui->itemManagerWidget->current_item())
        {
            return;
        }

        _node_variables.emplace_back();
        reload_variables();
    }

    void material_definition_manager::slot_save_clicked()
    {
        auto asset = get_current_asset();

        const auto mat_slots = _ui->spinBox_MatTexSlots->value();
        const auto node_slots = _ui->spinBox_NodeTexSlots->value();

        engine::material_definition new_def;
        new_def.set_material_texture_slot_count(mat_slots);
        new_def.set_node_texture_slot_count(node_slots);

        for (const auto& var : _material_variables)
        {
            new_def.add_material_variable(var);
        }

        for (const auto& var : _node_variables)
        {
            new_def.add_node_variable(var);
        }

        asset->set_definition(std::move(new_def));
        asset->save();
    }

    void material_definition_manager::slot_custom_types_clicked()
    {
        show_error_message("Not implemented!", this);
    }

    void material_definition_manager::slot_mat_glsl_struct_clicked()
    {
        const auto asset = get_current_asset();
        auto def_copy = asset->get_definition();

        def_copy.clear_material_variables();
        for (const auto& mat_var : _material_variables)
        {
            def_copy.add_material_variable(mat_var);
        }

        const auto struct_text = def_copy.create_material_uniform_glsl_struct();
        auto* dialog =
            new text_output_dialog("Output", "Generated shader struct", QString::fromStdString(struct_text), this);
        dialog->resize(this->size());
        dialog->exec();
    }

    void material_definition_manager::slot_node_glsl_struct_clicked()
    {
        const auto asset = get_current_asset();
        auto def_copy = asset->get_definition();

        def_copy.clear_node_variables();
        for (const auto& node_var : _node_variables)
        {
            def_copy.add_node_variable(node_var);
        }

        const auto struct_text = def_copy.create_node_uniform_glsl_struct();
        auto* dialog =
            new text_output_dialog("Output", "Generated shader struct", QString::fromStdString(struct_text), this);
        dialog->resize(this->size());
        dialog->exec();
    }

    void material_definition_manager::slot_mat_cpp_struct_clicked()
    {
        const auto asset = get_current_asset();
        auto def_copy = asset->get_definition();

        def_copy.clear_material_variables();
        for (const auto& mat_var : _material_variables)
        {
            def_copy.add_material_variable(mat_var);
        }

        const auto struct_text = def_copy.create_material_uniform_cpp_struct();
        auto* dialog = new text_output_dialog("Output", "Generated C++ struct", QString::fromStdString(struct_text), this);
        dialog->resize(this->size());
        dialog->exec();
    }

    void material_definition_manager::slot_node_cpp_struct_clicked()
    {
        const auto asset = get_current_asset();
        auto def_copy = asset->get_definition();

        def_copy.clear_node_variables();
        for (const auto& node_var : _node_variables)
        {
            def_copy.add_node_variable(node_var);
        }

        const auto struct_text = def_copy.create_node_uniform_cpp_struct();
        auto* dialog = new text_output_dialog("Output", "Generated C++ struct", QString::fromStdString(struct_text), this);
        dialog->resize(this->size());
        dialog->exec();
    }
} // namespace cathedral::editor