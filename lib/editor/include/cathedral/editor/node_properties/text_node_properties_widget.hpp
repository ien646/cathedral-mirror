#pragma once

#include "cathedral/engine/nodes/text_node.hpp"
#include "node_properties_widget.hpp"

namespace cathedral::editor
{
    class text_node_properties_widget final : public node_properties_widget
    {
    public:
        text_node_properties_widget(
            project::project* project,
            engine::scene& scene,
            engine::text_node* text_node,
            QWidget* parent);

    private:
        engine::text_node* _node = nullptr;
    };
}