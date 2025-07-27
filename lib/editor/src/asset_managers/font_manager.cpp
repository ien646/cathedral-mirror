#include "cathedral/editor/asset_managers/dialogs/new_font_dialog.hpp"

#include <QFormLayout>
#include <cathedral/editor/asset_managers/font_manager.hpp>

#include <cathedral/editor/utils.hpp>
#include <cathedral/project/project.hpp>

#include <QHBoxLayout>
#include <QLabel>
#include <QListWidget>
#include <QPushButton>
#include <QSpinBox>

namespace cathedral::editor
{
    font_manager::font_manager(project::project& pro, QWidget* parent)
        : QMainWindow(parent)
        , resource_manager_base(&pro)
        , _project(pro)
    {
        auto* main_widget = new QWidget;
        setCentralWidget(main_widget);

        auto* main_layout = new QHBoxLayout;
        main_widget->setLayout(main_layout);

        auto* list_layout = new QVBoxLayout;

        _item_manager = new item_manager(this);
        list_layout->addWidget(_item_manager);

        main_layout->addLayout(list_layout);

        _atlas_label = new QLabel("image");
        _atlas_label->setMinimumSize(100, 100);
        _atlas_label->setAlignment(Qt::AlignCenter);
        main_layout->addWidget(_atlas_label, 1);

        auto* atlas_gen_form = new QFormLayout;

        auto* starting_char_spinbox = new QSpinBox;
        starting_char_spinbox->setValue(0);
        starting_char_spinbox->setMinimum(0);
        atlas_gen_form->addRow("Starting character", starting_char_spinbox);

        auto* glyph_size_spinbox = new QSpinBox;
        glyph_size_spinbox->setValue(32);
        glyph_size_spinbox->setMinimum(0);
        atlas_gen_form->addRow("Glyph size", glyph_size_spinbox);

        auto* atlas_size_spinbox = new QSpinBox;
        atlas_size_spinbox->setValue(1024);
        atlas_size_spinbox->setMinimum(128);
        atlas_gen_form->addRow("Atlas size", atlas_size_spinbox);

        reload_item_list();

        connect(_item_manager, &item_manager::add_clicked, this, [this] { handle_add_clicked(); });
        connect(_item_manager, &item_manager::rename_clicked, this, [this] { handle_rename_clicked(); });
        connect(_item_manager, &item_manager::delete_clicked, this, [this] { handle_remove_clicked(); });
        connect(_item_manager, &item_manager::item_selection_changed, this, [this] { handle_item_selection_changed(); });
    }

    item_manager* font_manager::get_item_manager_widget()
    {
        return _item_manager;
    }

    const item_manager* font_manager::get_item_manager_widget() const
    {
        return _item_manager;
    }

    void font_manager::handle_add_clicked()
    {
        QStringList existing_names;
        for (const auto& font : _project.font_assets() | std::views::keys)
        {
            existing_names << QSTR(font);
        }

        auto* dialog = new new_font_dialog(this, existing_names);
        if (dialog->exec() == QDialog::Accepted)
        {
            const engine::font_data data = engine::generate_font_data(
                dialog->result_font().toStdString(),
                dialog->result_glyph_height(),
                dialog->result_atlas_size(),
                dialog->char_offset());

            const auto name = dialog->result_name().toStdString();
            const auto abs_path = _project.name_to_abspath<project::font_asset>(name);

            const auto asset = std::make_shared<project::font_asset>(&_project, abs_path);
            asset->mark_as_manually_loaded();
            asset->set_atlas_size({ data.atlas_image->width(), data.atlas_image->height() });
            asset->set_char_offset(data.char_offset);
            asset->set_glyph_boundind_box(data.glyph_bounding_box_size);
            asset->set_glyph_rects(data.glyph_rects);
            asset->save_atlas(*data.atlas_image);
            asset->save();

            _project.reload_font_assets();
            reload_item_list();
        }
    }

    void font_manager::handle_rename_clicked()
    {
        const auto rename_result = rename_asset();
        if (rename_result.has_value())
        {
            log_warning("[TODO] Font manager: unhandled rename propagation");
        }
    }

    void font_manager::handle_remove_clicked()
    {
        const auto delete_result = delete_asset();
        if (delete_result.has_value())
        {
            log_warning("[TODO] Font manager: unhandled delete propagation");
        }
    }

    void font_manager::handle_item_selection_changed()
    {
        const auto asset = get_current_asset();
        if (asset != nullptr)
        {
            _atlas_image = QImage(
                QSize{ static_cast<int>(asset->atlas_size().x), static_cast<int>(asset->atlas_size().y) },
                QImage::Format_Grayscale8);
            auto* data_ptr = _atlas_image.bits();

            const auto atlas = asset->load_atlas();

            std::memcpy(data_ptr, atlas.data(), atlas.size());

            _atlas_label->setText({});
            _atlas_label->setPixmap(
                QPixmap::fromImage(_atlas_image)
                    .scaled(
                        _atlas_label->width(),
                        _atlas_label->height(),
                        Qt::AspectRatioMode::KeepAspectRatio,
                        Qt::SmoothTransformation));
        }
    }

    void font_manager::resizeEvent(QResizeEvent* event)
    {
        QMainWindow::resizeEvent(event);
        if (!_atlas_image.isNull())
        {
            _atlas_label->setPixmap(
                QPixmap::fromImage(_atlas_image)
                    .scaled(_atlas_label->size(), Qt::AspectRatioMode::KeepAspectRatio, Qt::SmoothTransformation));
        }
    }
} // namespace cathedral::editor