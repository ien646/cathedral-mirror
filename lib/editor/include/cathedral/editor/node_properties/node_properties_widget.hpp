#pragma once

#include <QWidget>

class QVBoxLayout;

namespace cathedral::project
{
    class project;
}

namespace cathedral::engine
{
    class node;
}

namespace cathedral::editor
{
    class transform_widget;

    class node_properties_widget : public QWidget
    {
    public:
    node_properties_widget(project::project* pro, QWidget* parent, engine::node* node);

    private:
        project::project* _project;
        engine::node* _node = nullptr;

        QVBoxLayout* _main_layout = nullptr;
        transform_widget* _transform_widget = nullptr;

        void init_ui();

        void update_transform_widget();
    };
} // namespace cathedral::editor