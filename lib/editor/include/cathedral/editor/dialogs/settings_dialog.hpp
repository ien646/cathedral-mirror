#pragma once

#include <cathedral/core.hpp>

#include <QDialog>

FORWARD_CLASS(cathedral::project, project);

namespace cathedral::editor
{
    class settings_dialog final : QDialog
    {
    public:
        settings_dialog(project::project& pro, QWidget* parent);

    private:
        project::project& _project;
    };
} // namespace cathedral::editor