#pragma once

#include <cathedral/engine/nodes/camera3d_node.hpp>
#include <cathedral/editor/common/transform_widget.hpp>

#include <QVBoxLayout>
#include <QWidget>

namespace cathedral::editor
{
    class camera3d_properties_widget : public QWidget
    {
    public:
        camera3d_properties_widget(QWidget* parent);
        void set_node(engine::camera3d_node* node);

        void paintEvent(QPaintEvent* ev) override;

    private:
        QVBoxLayout* _main_layout = nullptr;
        transform_widget* _transform_widget = nullptr;
        sliding_float* _fov_slider = nullptr;
        engine::camera3d_node* _node = nullptr;

        void update_transform_widget();
    };
} // namespace cathedral::editor