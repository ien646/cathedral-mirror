#pragma once

#include <cathedral/core.hpp>

#include <QDialog>

FORWARD_CLASS(cathedral::project, project);

namespace cathedral::editor
{
    class settings_dialog final : public QDialog
    {
    public:
        settings_dialog(project::project& pro, QWidget* parent);

    private:
        project::project& _project;
        bool _settings_changed = false;

        QWidget* init_engine_tab();

        void closeEvent(QCloseEvent* event) override;
    };
} // namespace cathedral::editor