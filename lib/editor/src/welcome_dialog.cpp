#include <cathedral/editor/welcome_dialog.hpp>

#include <cathedral/editor/common/message.hpp>
#include <cathedral/editor/common/text_input_dialog.hpp>

#include <cathedral/project/project.hpp>

#include "ui_welcome_dialog.h"

#include <QFileDialog>

namespace cathedral::editor
{
    welcome_dialog::welcome_dialog(QWidget* parent)
        : QDialog(parent)
        , _ui(new Ui::welcome_dialog)
    {
        _ui->setupUi(this);

        connect(_ui->pushButton_NewProject, &QPushButton::clicked, this, [this] {
            QFileDialog file_dialog(this, "Select directory to create a new project");
            file_dialog.setFileMode(QFileDialog::FileMode::Directory);
            if (file_dialog.exec() == QDialog::Rejected)
            {
                return;
            }

            const auto& selected_path = file_dialog.selectedFiles().at(0).toStdString();
            QDir dir(QString::fromStdString(selected_path));
            if (!dir.isEmpty() && !show_confirm_dialog("Selected directory is not empty, continue?", this))
            {
                return;
            }

            auto* name_dialog = new text_input_dialog(this, "Give your new project a name", "Name: ", false);
            if (!name_dialog->exec())
            {
                return;
            }

            const auto& name = name_dialog->result().toStdString();

            _project = std::make_shared<project::project>(project::project::create(selected_path, name));
            accept();
        });

        connect(_ui->pushButton_OpenProject, &QPushButton::clicked, this, [this] {
            QFileDialog file_dialog(this, "Select directory containing an existing project");
            file_dialog.setFileMode(QFileDialog::FileMode::Directory);
            if (file_dialog.exec() == QDialog::Rejected)
            {
                return;
            }

            const auto& project_path = file_dialog.selectedFiles().at(0).toStdString();
            const auto& project_file_path = std::filesystem::path(project_path) / ".cathedral";
            if (!std::filesystem::exists(project_file_path))
            {
                show_error_message("Project file not found!");
                return;
            }

            _project = std::make_shared<project::project>();
            const auto load_result = _project->load_project(project_path);
            CRITICAL_CHECK(load_result == project::load_project_status::OK);

            accept();
        });
    }

    std::shared_ptr<project::project> welcome_dialog::project()
    {
        return _project;
    }
} // namespace cathedral::editor