#pragma once

#include <cathedral/editor/asset_managers/resource_manager_base.hpp>

#include <cathedral/project/assets/texture_asset.hpp>

#include <QMainWindow>

FORWARD_CLASS(cathedral::project, project);
FORWARD_CLASS(Ui, texture_manager);

namespace cathedral::editor
{
    class texture_manager
        : public QMainWindow
        , public resource_manager_base<project::texture_asset>
    {
        Q_OBJECT

    public:
        texture_manager(project::project* pro, QWidget* parent, bool allow_select = false);

        item_manager* get_item_manager_widget() override;
        const item_manager* get_item_manager_widget() const override;

    signals:
        void texture_selected(std::shared_ptr<project::texture_asset> asset);

    private:
        Ui::texture_manager* _ui = nullptr;
        bool _allow_select = false;
        uint32_t _current_mip_index = std::numeric_limits<uint32_t>::max();
        QImage _current_image;
        std::atomic_int _image_update_sequence = 0;

        void reload_current_image(bool force = false);
        void update_pixmap(const QImage& image);

        void showEvent(QShowEvent* ev) override;
        void resizeEvent(QResizeEvent* ev) override;

    private slots:
        void slot_add_texture();
        void slot_rename_texture();
        void slot_delete_texture();
        void slot_selected_texture_changed(std::optional<QString> selected);
    };
} // namespace cathedral::editor