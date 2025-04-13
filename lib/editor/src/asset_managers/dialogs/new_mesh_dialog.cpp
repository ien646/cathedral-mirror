#include <cathedral/editor/asset_managers/dialogs/new_mesh_dialog.hpp>

#include <cathedral/editor/common/message.hpp>
#include <cathedral/editor/common/path_selector.hpp>

#include <cathedral/engine/mesh.hpp>

#include <QFormLayout>
#include <QLineEdit>
#include <QPushButton>

#include <filesystem>

namespace cathedral::editor
{
    new_mesh_dialog::new_mesh_dialog(QStringList banned_names, QWidget* parent)
        : QDialog(parent)
        , _banned_names(std::move(banned_names))
    {
        setMinimumWidth(250);

        auto* name_edit = new QLineEdit("new_mesh");

        auto* path_edit = new path_selector(path_selector_mode::FILE);

        auto* create_button = new QPushButton("Create");

        auto* layout = new QFormLayout;
        layout->addRow("Name: ", name_edit);
        layout->addRow("Path: ", path_edit);
        layout->addRow("", create_button);

        setLayout(layout);

        connect(create_button, &QPushButton::clicked, this, [this, name_edit, path_edit] {
            if (name_edit->text().isEmpty())
            {
                show_error_message("Empty names are not allowed");
                return;
            }

            if (_banned_names.contains(name_edit->text()))
            {
                show_error_message("Mesh with specified name already exists");
                return;
            }

            if (!std::filesystem::exists(path_edit->text().toStdString()))
            {
                show_error_message("Specified file was not found");
                return;
            }

            const auto path = path_edit->text();
            try
            {
                const engine::mesh mesh(path.toStdString());
            }
            catch (const std::runtime_error& ex)
            {
                show_error_message(std::string{ "Unable to open mesh file:\n" } + ex.what());
                return;
            }

            _name = name_edit->text();
            _path = path;
            accept();
        });
    }
} // namespace cathedral::editor