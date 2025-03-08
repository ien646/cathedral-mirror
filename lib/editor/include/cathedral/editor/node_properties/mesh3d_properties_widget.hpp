#pragma once

#include <QWidget>

class QVBoxLayout;

namespace cathedral::engine
{
    class mesh3d_node;
}

namespace cathedral::project
{
    class project;
}

namespace cathedral::editor
{
    class material_selector;
    class mesh_selector;
    class transform_widget;

    class mesh3d_properties_widget : public QWidget
    {
    public:
        mesh3d_properties_widget(project::project* pro, QWidget* parent, engine::mesh3d_node* node);

    private:
        project::project* _project;
        engine::mesh3d_node* _node = nullptr;

        QVBoxLayout* _main_layout = nullptr;
        transform_widget* _transform_widget = nullptr;
        mesh_selector* _mesh_selector = nullptr;
        material_selector* _material_selector = nullptr;

        void init_ui();

        void update_transform_widget();
    };
} // namespace cathedral::editor