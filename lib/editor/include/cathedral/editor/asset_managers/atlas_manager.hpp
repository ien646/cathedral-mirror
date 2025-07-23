#pragma once

#include <cathedral/editor/asset_managers/resource_manager_base.hpp>
#include <cathedral/project/assets/atlas_asset.hpp>

#include <QMainWindow>

FORWARD_CLASS(cathedral::editor, atlas_viewer);
FORWARD_CLASS_INLINE(QBoxLayout);
FORWARD_CLASS_INLINE(QFrame);

namespace cathedral::editor
{
    class atlas_manager final
        : public QMainWindow
        , public resource_manager_base<project::atlas_asset>
    {
        Q_OBJECT

    public:
        atlas_manager(project::project& pro, engine::scene& scene, QWidget* parent, bool allow_select = false);

    private:
        project::project& _project;
        engine::scene& _scene;
        bool _allow_select;

        item_manager* _item_manager = nullptr;
        QFrame* _placeholder_frame = nullptr;
        QFrame* _edit_frame = nullptr;
        QBoxLayout* _edit_layout = nullptr;
        atlas_viewer* _atlas_viewer = nullptr;

        item_manager* get_item_manager_widget() override;
        const item_manager* get_item_manager_widget() const override;

        void handle_add_clicked();
        void handle_rename_clicked();
        void handle_delete_clicked();
    };
} // namespace cathedral::editor