#pragma once

#include <cathedral/core.hpp>

#include <cathedral/editor/asset_managers/resource_manager_base.hpp>

#include <QMainWindow>

FORWARD_CLASS(cathedral::project, mesh_asset);
FORWARD_CLASS(cathedral::project, project);
FORWARD_CLASS(Ui, mesh_manager);

namespace cathedral::editor
{
    class mesh_manager
        : public QMainWindow
        , public resource_manager_base<project::mesh_asset>
    {
    public:
        mesh_manager(project::project& pro, QWidget* parent);

        item_manager* get_item_manager_widget() override;

    private:
        Ui::mesh_manager* _ui = nullptr;
    };
} // namespace cathedral::editor
