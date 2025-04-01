#include <cathedral/editor/asset_managers/dialogs/new_shader_dialog.hpp>

#include <cathedral/gfx/shader.hpp>

#include <cathedral/editor/common/message.hpp>

#include <magic_enum.hpp>

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

    new_shader_dialog::new_shader_dialog(QWidget* parent, bool allow_empty)
        : QDialog(parent)
    {
        setWindowTitle("New shader");

        auto* main_layout = new QVBoxLayout;
        setLayout(main_layout);

        auto* top_layout = new QFormLayout;
        main_layout->addLayout(top_layout, 1);

        auto* bottom_layout = new QHBoxLayout;
        main_layout->addLayout(bottom_layout);

        QStringList shader_types_list;
        for (const auto& name : magic_enum::enum_names<gfx::shader_type>())
        {
            shader_types_list << QString::fromStdString(std::string{ name });
        }
        auto* type_combo = new QComboBox;
        type_combo->addItems(shader_types_list);
        top_layout->addRow("Type: ", type_combo);

        auto* name_edit = new QLineEdit;
        name_edit->setText("new_shader");
        top_layout->addRow("Name: ", name_edit);

        bottom_layout->addStretch(1);

        auto* accept_button = new QPushButton;
        accept_button->setText("Create");
        bottom_layout->addWidget(accept_button, 0);

        adjustSize();

        connect(accept_button, &QPushButton::clicked, this, [this, allow_empty, name_edit, type_combo] {
            if (name_edit->text().isEmpty())
            {
                if (allow_empty)
                {
                    _result = {};
                    _type = type_combo->currentText() == EMPTY_VALUE ? QString{} : type_combo->currentText();
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
                _type = type_combo->currentText() == EMPTY_VALUE ? QString{} : type_combo->currentText();
                accept();
            }
        });
    }
} // namespace cathedral::editor