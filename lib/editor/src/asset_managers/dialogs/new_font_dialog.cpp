#include <cathedral/editor/asset_managers/dialogs/new_font_dialog.hpp>

#include <QFormLayout>
#include <QLineEdit>
#include <QPushButton>
#include <QSpinBox>

namespace cathedral::editor
{
    new_font_dialog::new_font_dialog(QWidget* parent, const QStringList& forbidden_names)
        : QDialog(parent)
        , _forbidden_names(forbidden_names)
    {
        setMinimumWidth(420);

        auto* main_layout = new QFormLayout;
        setLayout(main_layout);

        auto* name_edit = new QLineEdit;
        main_layout->addRow("Name:", name_edit);

        auto* font_file = new QLineEdit;
        main_layout->addRow("Font:", font_file);

        auto* browse_button = new QPushButton("Browse...");
        main_layout->addRow(browse_button);

        auto* atlas_size_x = new QSpinBox;
        auto* atlas_size_y = new QSpinBox;

        atlas_size_x->setMinimum(64);
        atlas_size_y->setMinimum(64);
        atlas_size_x->setMaximum(8192);
        atlas_size_y->setMaximum(8192);

        atlas_size_x->setValue(1024);
        atlas_size_y->setValue(1024);

        main_layout->addRow("Atlas width:", atlas_size_x);
        main_layout->addRow("Atlas height:", atlas_size_y);

        auto* char_gen_offset_spinbox = new QSpinBox;
        char_gen_offset_spinbox->setMinimum(0);
        main_layout->addRow("Character offset:", char_gen_offset_spinbox);

        auto* gen_button = new QPushButton("Generate");

        main_layout->addRow(gen_button);
    }
} // namespace cathedral::editor