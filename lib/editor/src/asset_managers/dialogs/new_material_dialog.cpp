#include <cathedral/editor/asset_managers/dialogs/new_material_dialog.hpp>

#include <cathedral/editor/common/message.hpp>

#include <QComboBox>
#include <QFormLayout>
#include <QLineEdit>
#include <QPushButton>

namespace cathedral::editor
{
    new_material_dialog::new_material_dialog(
        QStringList banned_names,
        QWidget* parent)
        : QDialog(parent)
        , _banned_names(std::move(banned_names))
    {
        setWindowTitle("New material");

        int placeholder_increment = 0;
        QString placeholder_name = "new_material";
        while (_banned_names.contains(placeholder_name))
        {
            placeholder_name = "new_material" + QString::number(placeholder_increment++);
        }
        _name_edit = new QLineEdit(placeholder_name);

        auto* create_button = new QPushButton("Create");

        auto* main_layout = new QFormLayout;
        main_layout->addRow("Name: ", _name_edit);
        main_layout->addRow("", create_button);

        setLayout(main_layout);

        connect(create_button, &QPushButton::clicked, this, &SELF::handle_create_clicked);
    }

    void new_material_dialog::handle_create_clicked()
    {
        const auto name = _name_edit->text();
        if (name.isEmpty())
        {
            show_error_message("Name can't be empty");
            return;
        }

        if (_banned_names.contains(name))
        {
            show_error_message("Material with specified name already exists");
            return;
        }

        _name = name;
        accept();
    }
} // namespace cathedral::editor