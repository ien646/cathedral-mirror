#pragma once

#include <cathedral/core.hpp>

#include <cathedral/editor/asset_managers/resource_manager_base.hpp>

#include <QMainWindow>

FORWARD_CLASS(cathedral::project, mesh_asset);
FORWARD_CLASS(cathedral::project, project);
FORWARD_CLASS(Ui, mesh_manager); //NOLINT

namespace cathedral::editor
{
    class mesh_manager
        : public QMainWindow
        , public resource_manager_base<project::mesh_asset>
    {
        Q_OBJECT

    public:
        mesh_manager(project::project* pro, QWidget* parent, bool allow_select = false);

        item_manager* get_item_manager_widget() override;
        const item_manager* get_item_manager_widget() const override;

    signals:
        void closed();
        void mesh_selected(std::shared_ptr<project::mesh_asset> asset);

    private:
        Ui::mesh_manager* _ui = nullptr;
        bool _allow_select = false;

        void showEvent(QShowEvent* ev) override;
        void closeEvent(QCloseEvent* ev) override;

        void handle_add_mesh_clicked();
        void handle_rename_mesh_clicked();
        void handle_delete_mesh_clicked();
        void handle_mesh_selection_changed(std::optional<QString> selected);
    };
} // namespace cathedral::editor
