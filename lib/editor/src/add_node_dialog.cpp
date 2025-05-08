#include <cathedral/editor/add_node_dialog.hpp>

#include <cathedral/editor/common/message.hpp>

#include <cathedral/engine/node_type.hpp>

#include <QComboBox>
#include <QFormLayout>
#include <QLineEdit>
#include <QPushButton>
#include <QVBoxLayout>

#include <magic_enum.hpp>

namespace cathedral::editor
{
    add_node_dialog::add_node_dialog(QWidget* parent)
        : QDialog(parent)
    {
        setWindowModality(Qt::WindowModality::ApplicationModal);

        auto* main_layout = new QVBoxLayout;
        setLayout(main_layout);

        auto* form_layout = new QFormLayout;
        main_layout->addLayout(form_layout);

        auto* type_combo = new QComboBox;
        for (const auto& [type, name] : magic_enum::enum_entries<engine::node_type>())
        {
            type_combo->addItem(QString::fromStdString(std::string{ name }), static_cast<uint8_t>(type));
        }
        form_layout->addRow("Type: ", type_combo);

        auto* name_line_edit = new QLineEdit;
        form_layout->addRow("Name: ", name_line_edit);

        auto* buttons_layout = new QHBoxLayout;
        main_layout->addLayout(buttons_layout);

        auto* cancel_button = new QPushButton("Cancel");
        auto* add_button = new QPushButton("Add");
        buttons_layout->addWidget(add_button);
        buttons_layout->addWidget(cancel_button);

        connect(add_button, &QPushButton::clicked, this, [=, this] {
            if (name_line_edit->text().isEmpty())
            {
                show_error_message("Empty name is not a valid name", this);
                return;
            }

            if (name_line_edit->text().startsWith("__"))
            {
                show_error_message("Node names starting with double underscore '__' are reserved for implicit nodes");
                return;
            }

            _result.name = name_line_edit->text().toStdString();
            _result.type = static_cast<engine::node_type>(type_combo->itemData(type_combo->currentIndex()).toInt());

            accept();
        });

        connect(cancel_button, &QPushButton::clicked, this, [=, this] { reject(); });
    }
} // namespace cathedral::editor