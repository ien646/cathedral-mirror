#pragma once

#include <cathedral/core.hpp>

#include <cathedral/editor/asset_managers/resource_manager_base.hpp>
#include <cathedral/project/assets/font_asset.hpp>

#include <QMainWindow>

FORWARD_CLASS(cathedral::project, project);
FORWARD_CLASS_INLINE(QListWidget)

namespace cathedral::editor
{
    class font_manager final
        : public QMainWindow
        , public resource_manager_base<project::font_asset>
    {
    public:
        explicit font_manager(project::project& pro, QWidget* parent = nullptr);

    protected:
        item_manager* get_item_manager_widget() override;
        const item_manager* get_item_manager_widget() const override;

    private:
        project::project& _project;

        item_manager* _item_manager = nullptr;

        void handle_add_clicked();
        void handle_rename_clicked();
        void handle_remove_clicked();
    };
} // namespace cathedral::editor