#pragma once

#include <cathedral/project/project.hpp>

#include <QDialog>

namespace Ui
{
    class welcome_dialog;
}

namespace cathedral::editor
{
    class welcome_dialog : public QDialog
    {
    public:
        welcome_dialog(QWidget* parent = nullptr);

        std::shared_ptr<project::project> project();

    private:
        Ui::welcome_dialog* _ui = nullptr;
        std::shared_ptr<project::project> _project;
    };
}