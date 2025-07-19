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
        , _project(pro)
    {
        auto* main_widget = new QWidget;
        setCentralWidget(main_widget);

        auto* main_layout = new QHBoxLayout;
        main_widget->setLayout(main_layout);

        auto* list_layout = new QVBoxLayout;

        _list_widget = new QListWidget;

        for (const auto& font : _project.available_fonts())
        {
            _list_widget->addItem(QSTR(font));
        }

        list_layout->addWidget(_list_widget);

        auto* buttons_layout = new QHBoxLayout;
        auto* import_button = new QPushButton("Import");
        auto* rename_button = new QPushButton("Rename");
        auto* delete_button = new QPushButton("Delete");
        buttons_layout->addWidget(import_button);
        buttons_layout->addWidget(rename_button);
        buttons_layout->addWidget(delete_button);

        list_layout->addLayout(buttons_layout);

        main_layout->addLayout(list_layout);

        auto* image_label = new QLabel("image");
        main_layout->addWidget(image_label);

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

        auto* gen_atlas
    }
} // namespace cathedral::editor