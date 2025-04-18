#pragma once

#include <cathedral/editor/common/float_grid.hpp>
#include <cathedral/editor/common/sliding_float.hpp>
#include <cathedral/editor/common/transform_widget.hpp>

#include <cathedral/engine/nodes/camera2d_node.hpp>

#include <QWidget>

namespace cathedral::editor
{
    class camera2d_properties_widget : public QWidget
    {
        Q_OBJECT

    public:
        camera2d_properties_widget(QWidget* parent, engine::camera2d_node* node);

        void paintEvent(QPaintEvent* ev) override;

    private:
        engine::camera2d_node* _node = nullptr;
        transform_widget* _transform_widget = nullptr;
        sliding_float* _znear_slider = nullptr;
        sliding_float* _zfar_slider = nullptr;

        void update_ui();
        void update_transform_widget();
    };
} // namespace cathedral::editor