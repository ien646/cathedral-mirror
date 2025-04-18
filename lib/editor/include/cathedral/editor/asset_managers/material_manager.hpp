#pragma once

#include <cathedral/editor/asset_managers/resource_manager_base.hpp>

#include <QMainWindow>

FORWARD_CLASS(Ui, material_manager); //NOLINT
FORWARD_CLASS(cathedral::engine, scene);
FORWARD_CLASS(cathedral::project, project);

namespace cathedral::editor
{
    class material_manager final
        : public QMainWindow
        , public resource_manager_base<project::material_asset>
    {
        Q_OBJECT

    public:
        material_manager(project::project* pro, std::shared_ptr<engine::scene> scene, QWidget* parent, bool allow_select = false);

        item_manager* get_item_manager_widget() override;
        const item_manager* get_item_manager_widget() const override;

    signals:
        void closed();
        void material_selected(std::shared_ptr<project::material_asset> asset);

    private:
        Ui::material_manager* _ui;
        std::shared_ptr<engine::scene> _scene;
        bool _allow_select = false;

        void reload_material_props();

        void init_shaders_tab();
        void init_variables_tab();
        void init_textures_tab();

        void showEvent(QShowEvent* ev) override;
        void closeEvent(QCloseEvent* ev) override;

        void handle_add_material_clicked();
        void handle_rename_material_clicked();
        void handle_delete_material_clicked();
        void handle_material_selection_changed(std::optional<QString> selected);

        void handle_texture_slot_clicked(uint32_t slot);

        void handle_material_props_changed();
    };
} // namespace cathedral::editor