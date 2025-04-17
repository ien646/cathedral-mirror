#pragma once

#include <cathedral/editor/common/float_grid.hpp>
#include <cathedral/editor/common/transform_widget.hpp>

#include <cathedral/engine/nodes/camera2d_node.hpp>

#include <QWidget>
#include <qtmetamacros.h>

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
        float_grid* _bounds_x = nullptr;
        float_grid* _bounds_y = nullptr;
        float_grid* _bounds_z = nullptr;

        void init_ui();
        void update_transform_widget();
    };
} // namespace cathedral::editor