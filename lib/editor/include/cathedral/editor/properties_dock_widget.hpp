#pragma once

#include <cathedral/engine/nodes/camera3d_node.hpp>
#include <cathedral/engine/nodes/mesh3d_node.hpp>

#include <QDockWidget>
#include <QScrollArea>

namespace cathedral::editor
{
    class properties_dock_widget : public QDockWidget
    {
    public:
        properties_dock_widget(QWidget* parent = nullptr);

        void clear_node();
        void set_node(engine::mesh3d_node* node);
        void set_node(engine::camera3d_node* node);

    private:
        QScrollArea* _scroll_area = nullptr;
        QWidget* _properties_widget = nullptr;

        template<typename TWidget, typename TNode>
        void set_node_generic(TWidget* widget, TNode* node);
    };
} // namespace cathedral::editor