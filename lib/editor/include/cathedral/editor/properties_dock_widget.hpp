#pragma once

#include <QDockWidget>

class QScrollArea;

namespace cathedral::engine
{
    class camera3d_node;
    class mesh3d_node;
} // namespace cathedral::engine

namespace cathedral::project
{
    class project;
}

namespace cathedral::editor
{
    class properties_dock_widget : public QDockWidget
    {
    public:
        properties_dock_widget(project::project* pro, QWidget* parent = nullptr);

        void clear_node();
        void set_node(engine::mesh3d_node* node);
        void set_node(engine::camera3d_node* node);

    private:
        project::project* _project;
        QScrollArea* _scroll_area = nullptr;
        QWidget* _properties_widget = nullptr;

        template <typename TWidget>
        void set_node_generic(TWidget* widget);
    };
} // namespace cathedral::editor