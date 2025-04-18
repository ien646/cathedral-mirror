#pragma once

#include <cathedral/core.hpp>

#include <QWidget>
#include <qtmetamacros.h>

class QVBoxLayout;

FORWARD_CLASS(cathedral::engine, camera3d_node);

namespace cathedral::editor
{
    class sliding_float;
    class transform_widget;

    class camera3d_properties_widget : public QWidget
    {
        Q_OBJECT

    public:
        camera3d_properties_widget(QWidget* parent, engine::camera3d_node* node);

        void paintEvent(QPaintEvent* ev) override;

    private:
        transform_widget* _transform_widget = nullptr;
        sliding_float* _fov_slider = nullptr;
        sliding_float* _znear_slider = nullptr;
        sliding_float* _zfar_slider = nullptr;
        engine::camera3d_node* _node = nullptr;

        void update_ui();
        void update_transform_widget();
    };
} // namespace cathedral::editor