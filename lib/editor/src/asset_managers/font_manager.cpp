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

        auto* atlas_label = new QLabel("image");
        main_layout->addWidget(atlas_label);

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
} // namespace cathedral::editor