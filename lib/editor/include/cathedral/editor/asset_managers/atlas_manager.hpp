#pragma once

#include <cathedral/editor/asset_managers/resource_manager_base.hpp>
#include <cathedral/project/assets/atlas_asset.hpp>

#include <QMainWindow>

namespace cathedral::editor
{
    class atlas_manager final
        : public resource_manager_base<project::atlas_asset>
        , public QMainWindow
    {
    public:
        atlas_manager(project::project& pro, engine::scene& scene, QWidget* parent, bool allow_select = false);

    private:
        project::project& _project;
        engine::scene& _scene;
        bool _allow_select;

        item_manager* _item_manager = nullptr;

        item_manager* get_item_manager_widget() override;
        const item_manager* get_item_manager_widget() const override;
    };
} // namespace cathedral::editor