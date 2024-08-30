#pragma once

#include <cathedral/engine/nodes/mesh3d_node.hpp>
#include <cathedral/editor/common/path_selector.hpp>
#include <cathedral/editor/common/transform_widget.hpp>

#include <QVBoxLayout>
#include <QWidget>

namespace cathedral::editor
{
    class mesh3d_properties_widget : public QWidget
    {
    public:
        mesh3d_properties_widget(QWidget* parent);
        void set_node(engine::mesh3d_node* node);

        void paintEvent(QPaintEvent* ev) override;

    private:
        engine::mesh3d_node* _node = nullptr;

        QVBoxLayout* _main_layout = nullptr;
        transform_widget* _transform_widget = nullptr;
        path_selector* _mesh_selector = nullptr;

        void update_transform_widget();
    };
} // namespace cathedral::editor