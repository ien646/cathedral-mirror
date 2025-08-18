#pragma once

#include <cathedral/editor/asset_managers/resource_manager_base.hpp>

#include <QMainWindow>

FORWARD_CLASS(Ui, material_manager); // NOLINT
FORWARD_CLASS(cathedral::engine, scene);
FORWARD_CLASS(cathedral::project, project);
FORWARD_CLASS_INLINE(QTableWidget);

namespace cathedral::editor
{
    class material_manager final
        : public QMainWindow
        , public resource_manager_base<project::material_asset>
    {
        Q_OBJECT

    public:
        material_manager(project::project* pro, engine::scene& scene, QWidget* parent, bool allow_select = false);

        item_manager* get_item_manager_widget() override;
        const item_manager* get_item_manager_widget() const override;

    signals:
        void closed();
        void material_selected(std::shared_ptr<project::material_asset> asset);

    private:
        Ui::material_manager* _ui;
        engine::scene& _scene;
        bool _allow_select = false;

        QTableWidget* _material_uniform_table = nullptr;
        QTableWidget* _material_texture_table = nullptr;
        QTableWidget* _material_buffer_table = nullptr;
        QTableWidget* _node_uniform_table = nullptr;
        QTableWidget* _node_texture_table = nullptr;
        QTableWidget* _node_buffer_table = nullptr;

        void reload_material_props();

        void init_shaders_tab();
        void init_variables_tab();
        void init_textures_tab();

        void init_uniform_tables(const std::weak_ptr<engine::material>& material, const std::shared_ptr<project::material_asset>& asset);
        void init_texture_tables(std::weak_ptr<engine::material> material, std::shared_ptr<project::material_asset> asset);
        void init_buffer_tables(std::weak_ptr<engine::material> material, std::shared_ptr<project::material_asset> asset);

        void showEvent(QShowEvent* ev) override;
        void closeEvent(QCloseEvent* ev) override;

        void handle_add_material_clicked();
        void handle_rename_material_clicked();
        void handle_delete_material_clicked();
        void handle_material_selection_changed(const std::optional<QString>& selected);

        void handle_texture_slot_clicked(uint32_t slot);

        void handle_material_props_changed();
    };
} // namespace cathedral::editor