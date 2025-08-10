#include <cathedral/editor/node_properties/text_node_properties_widget.hpp>

namespace cathedral::editor
{
    text_node_properties_widget::text_node_properties_widget(
        project::project* project,
        engine::scene& scene,
        engine::text_node* text_node,
        QWidget* parent)
        : node_properties_widget(project, parent, text_node, false)
        , _node(text_node)
    {
        CRITICAL_CHECK_NOTNULL(_project);
        CRITICAL_CHECK_NOTNULL(_node);

        setObjectName("text_node_properties_widget");

        NOT_IMPLEMENTED();
    }
} // namespace cathedral::editor