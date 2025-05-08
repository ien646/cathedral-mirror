#pragma once

#include <cathedral/editor/node_properties/node_properties_widget.hpp>

FORWARD_CLASS(cathedral::editor, transform_widget);
FORWARD_CLASS(cathedral::engine, point_light_node);
FORWARD_CLASS(cathedral::engine, scene);
FORWARD_CLASS(cathedral::project, project);

FORWARD_CLASS_INLINE(QVBoxLayout);

namespace cathedral::editor
{
    class point_light_properties_widget final : public node_properties_widget
    {
    public:
        point_light_properties_widget(
            project::project* pro,
            std::shared_ptr<engine::scene> scene,
            QWidget* parent,
            engine::point_light_node* node);

    private:
        engine::point_light_node* _node;
    };
} // namespace cathedral::editor