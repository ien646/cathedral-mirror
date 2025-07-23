#pragma once

#include <cathedral/core.hpp>

#include <QGraphicsView>

FORWARD_CLASS(cathedral::engine, atlas);
FORWARD_CLASS(cathedral::project, project);

namespace cathedral::editor
{
    class atlas_viewer final : public QGraphicsView
    {
    public:
        atlas_viewer(project::project& pro, const std::string& atlas_name, QWidget* parent);

    private:
        project::project& _project;
        const std::string& _atlas_name;
        QGraphicsScene* _scene = nullptr;

        void init_scene();
    };
} // namespace cathedral::editor