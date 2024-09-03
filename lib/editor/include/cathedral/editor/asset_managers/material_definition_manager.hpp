#pragma once

#include <cathedral/editor/asset_managers/resource_manager_base.hpp>

#include <cathedral/engine/material_definition.hpp>

#include <QMainWindow>

FORWARD_CLASS(cathedral::project, material_definition_asset);
FORWARD_CLASS(cathedral::project, project);
FORWARD_CLASS(Ui, material_definition_manager);
FORWARD_CLASS_INLINE(QTableWidget);

namespace cathedral::editor
{
    class material_definition_manager
        : public QMainWindow
        , public resource_manager_base<project::material_definition_asset>
    {
        Q_OBJECT

    public:
        material_definition_manager(project::project& pro, QWidget* parent);

        item_manager* get_item_manager_widget() override;

    private:
        Ui::material_definition_manager* _ui = nullptr;

        std::vector<engine::shader_variable> _material_variables;
        std::vector<engine::shader_variable> _node_variables;

        void reload_variables();
        std::shared_ptr<project::material_definition_asset> get_current_asset();

        void set_row_for_variable(
            uint32_t row_index,
            QTableWidget* table_widget,
            std::vector<engine::shader_variable>& variables);
        void set_row_for_material_variable(uint32_t row_index);
        void set_row_for_node_variable(uint32_t row_index);

        void showEvent(QShowEvent* ev) override;

    private slots:
        void slot_selected_changed();

        void slot_add_definition_clicked();
        void slot_rename_definition_clicked();
        void slot_delete_definition_clicked();

        void slot_add_material_variable_clicked();
        void slot_add_node_variable_clicked();

        void slot_save_clicked();
        void slot_custom_types_clicked();

        void slot_mat_glsl_struct_clicked();
        void slot_node_glsl_struct_clicked();

        void slot_mat_cpp_struct_clicked();
        void slot_node_cpp_struct_clicked();
    };
} // namespace cathedral::editor