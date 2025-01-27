#pragma once

#include <cathedral/editor/asset_managers/resource_manager_base.hpp>

#include <QMainWindow>

FORWARD_CLASS(Ui, material_manager);
FORWARD_CLASS(cathedral::project, project);

namespace cathedral::editor
{
    class material_manager final
        : public QMainWindow
        , public resource_manager_base<project::material_asset>
    {
        Q_OBJECT

    public:
        material_manager(project::project& pro, QWidget* parent);

        item_manager* get_item_manager_widget() override;
        const item_manager* get_item_manager_widget() const override;

    private:
        Ui::material_manager* _ui;

        void reload_material_props();

        void init_shaders_tab();
        void init_textures_tab();

        void showEvent(QShowEvent* ev) override;

    private slots:
        void slot_add_material_clicked();
        void slot_rename_material_clicked();
        void slot_delete_material_clicked();
        void slot_material_selection_changed();
    };
} // namespace cathedral::editor