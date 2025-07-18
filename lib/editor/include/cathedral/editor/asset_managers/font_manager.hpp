#pragma once

#include <cathedral/core.hpp>

#include <QMainWindow>

FORWARD_CLASS(cathedral::project, project);
FORWARD_CLASS_INLINE(QListWidget)

namespace cathedral::editor
{
    class font_manager final : public QMainWindow
    {
    public:
        explicit font_manager(project::project& pro, QWidget* parent = nullptr);

    private:
        project::project& _project;
        QListWidget* _list_widget;
    };
} // namespace cathedral::editor