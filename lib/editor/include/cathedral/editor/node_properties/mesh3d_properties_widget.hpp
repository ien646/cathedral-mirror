#pragma once

#include <QWidget>

class QVBoxLayout;

namespace cathedral::engine
{
    class mesh3d_node;
}

namespace cathedral::editor
{
    class path_selector;
    class transform_widget;

    class mesh3d_properties_widget : public QWidget
    {
    public:
        mesh3d_properties_widget(QWidget* parent, engine::mesh3d_node* node);

    private:
        engine::mesh3d_node* _node = nullptr;

        QVBoxLayout* _main_layout = nullptr;
        transform_widget* _transform_widget = nullptr;
        path_selector* _mesh_selector = nullptr;

        void init_ui();

        void update_transform_widget();
    };
} // namespace cathedral::editor