#pragma once

#include <cathedral/editor/node_properties/node_properties_widget.hpp>

FORWARD_CLASS(cathedral::engine, directional_light_node);
FORWARD_CLASS(cathedral::engine, scene);

namespace cathedral::editor
{
    class directional_light_properties_widget final : public node_properties_widget
    {
    public:
        directional_light_properties_widget(
            project::project* pro,
            engine::scene& scene,
            QWidget* parent,
            engine::directional_light_node* node);

    private:
        engine::directional_light_node* _node = nullptr;
    };
}