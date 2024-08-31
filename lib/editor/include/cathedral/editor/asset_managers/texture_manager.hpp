#pragma once

#include <cathedral/editor/asset_managers/resource_manager_base.hpp>

#include <cathedral/project/assets/texture_asset.hpp>

#include <QMainWindow>

namespace cathedral::project
{
    class project;
}

namespace Ui
{
    class texture_manager;
}

namespace cathedral::editor
{
    class texture_manager
        : public QMainWindow
        , public resource_manager_base<project::texture_asset>
    {
        Q_OBJECT

    public:
        texture_manager(project::project& pro, QWidget* parent);

        item_manager* get_item_manager_widget() override;

    private:
        Ui::texture_manager* _ui = nullptr;
        uint32_t _current_mip_index = std::numeric_limits<uint32_t>::max();
        QImage _current_image = {};

        void reload_current_image(bool force = false);
        void update_pixmap(QImage image);

        void resizeEvent(QResizeEvent* ev) override;

    private slots:
        void slot_add_texture();
        void slot_rename_texture();
        void slot_delete_texture();
        void slot_selected_texture_changed();
    };
} // namespace cathedral::editor