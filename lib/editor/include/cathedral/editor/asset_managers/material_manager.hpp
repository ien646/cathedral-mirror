#pragma once

#include <cathedral/editor/asset_managers/resource_manager_base.hpp>

#include <QMainWindow>

namespace Ui
{
    class material_manager;
};

namespace cathedral::project
{
    class project;
}

namespace cathedral::editor
{
    class material_manager
        : public QMainWindow
        , public resource_manager_base<project::material_asset>
    {
        Q_OBJECT

    public:
        material_manager(project::project& pro, QWidget* parent);

        item_manager* get_item_manager_widget() override;

    private:
        Ui::material_manager* _ui;

        void init_shaders_tab();

    private slots:
        void slot_add_material_clicked();
        void slot_rename_material_clicked();
        void slot_delete_material_clicked();
    };
} // namespace cathedral::editor