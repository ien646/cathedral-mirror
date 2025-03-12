#pragma once

#include <cathedral/core.hpp>

#include <QWidget>

class QVBoxLayout;

FORWARD_CLASS(cathedral::engine, camera3d_node);

namespace cathedral::editor
{
    class sliding_float;
    class transform_widget;

    class camera3d_properties_widget : public QWidget
    {
    public:
        camera3d_properties_widget(QWidget* parent, engine::camera3d_node* node);

        void paintEvent(QPaintEvent* ev) override;

    private:
        QVBoxLayout* _main_layout = nullptr;
        transform_widget* _transform_widget = nullptr;
        sliding_float* _fov_slider = nullptr;
        engine::camera3d_node* _node = nullptr;

        void init_ui();
        void update_transform_widget();
    };
} // namespace cathedral::editor