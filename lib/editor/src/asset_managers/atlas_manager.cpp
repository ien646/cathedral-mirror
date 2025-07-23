#include <QHBoxLayout>
#include <cathedral/editor/asset_managers/atlas_manager.hpp>

namespace cathedral::editor
{
    atlas_manager::atlas_manager(project::project& pro, engine::scene& scene, QWidget* parent, bool allow_select)
        : resource_manager_base<cathedral::project::atlas_asset>(&_project, {})
        , QMainWindow(parent)
        , _project(pro)
        , _scene(scene)
        , _allow_select(allow_select)
    {
        auto* main_layout = new QHBoxLayout;

        _item_manager = new item_manager(this);
        main_layout->addWidget(_item_manager);

        auto* vis = new QWidget;
        main_layout->addWidget(vis);

        reload_item_list();
    }

    item_manager* atlas_manager::get_item_manager_widget()
    {
        return _item_manager;
    }

    const item_manager* atlas_manager::get_item_manager_widget() const
    {
        return _item_manager;
    }
} // namespace cathedral::editor