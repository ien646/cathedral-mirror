#pragma once

#include <cathedral/engine/material_definition.hpp>

#include <QMainWindow>
#include <QTableWidget>

namespace cathedral::project
{
    class material_definition_asset;
    class project;
} // namespace cathedral::project

namespace Ui
{
    class material_definition_manager;
}

namespace cathedral::editor
{
    class material_definition_manager : public QMainWindow
    {
        Q_OBJECT

    public:
        material_definition_manager(project::project& pro, QWidget* parent);

        void reload();

    private:
        project::project& _project;
        Ui::material_definition_manager* _ui = nullptr;

        std::vector<engine::material_definition::variable> _material_variables;
        std::vector<engine::material_definition::variable> _node_variables;

        void reload_variables();
        std::shared_ptr<project::material_definition_asset> get_current_asset();

        void set_row_for_variable(
            uint32_t row_index,
            QTableWidget* table_widget,
            std::vector<engine::material_definition::variable>& variables);
        void set_row_for_material_variable(uint32_t row_index);
        void set_row_for_node_variable(uint32_t row_index);

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