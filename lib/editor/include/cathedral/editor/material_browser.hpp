#pragma once

#include <QWidget>

FORWARD_CLASS(cathedral::project, project);

namespace cathedral::editor
{
    class material_browser : public QWidget
    {
    public:
        material_browser(project::project& project, QWidget* parent);

    private:
        project::project& _project;
    };
}