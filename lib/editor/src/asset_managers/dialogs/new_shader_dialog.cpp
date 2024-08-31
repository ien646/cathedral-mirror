#include <cathedral/editor/asset_managers/dialogs/new_shader_dialog.hpp>

#include <cathedral/editor/common/message.hpp>

#include <QComboBox>
#include <QFormLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QVBoxLayout>

namespace cathedral::editor
{
    constexpr const char* EMPTY_VALUE = "NONE";

    new_shader_dialog::new_shader_dialog(const QStringList& available_materials, QWidget* parent, bool allow_empty)
    {
        auto* main_layout = new QVBoxLayout;
        setLayout(main_layout);

        auto* top_layout = new QFormLayout;
        main_layout->addLayout(top_layout, 1);

        auto* bottom_layout = new QHBoxLayout;
        main_layout->addLayout(bottom_layout);

        auto sorted_matdefs = available_materials;
        sorted_matdefs.sort();

        auto* matdef_combo = new QComboBox;
        matdef_combo->addItem(EMPTY_VALUE);
        matdef_combo->addItems(sorted_matdefs);
        top_layout->addRow("Material definition: ", matdef_combo);

        auto* name_edit = new QLineEdit;
        name_edit->setText("new_shader");
        top_layout->addRow("Name: ", name_edit);

        bottom_layout->addStretch(1);

        auto* accept_button = new QPushButton;
        accept_button->setText("Accept");
        bottom_layout->addWidget(accept_button, 0);

        adjustSize();

        connect(accept_button, &QPushButton::clicked, this, [this, allow_empty, name_edit, matdef_combo] {
            if (name_edit->text().isEmpty())
            {
                if (allow_empty)
                {
                    _result = {};
                    _matdef = matdef_combo->currentText() == EMPTY_VALUE ? QString{} : matdef_combo->currentText();
                    accept();
                }
                else
                {
                    show_error_message("Invalid input. Empty text is not allowed", this);
                }
            }
            else
            {
                _result = name_edit->text();
                _matdef = matdef_combo->currentText() == EMPTY_VALUE ? QString{} : matdef_combo->currentText();
                accept();
            }
        });
    }
} // namespace cathedral::editor