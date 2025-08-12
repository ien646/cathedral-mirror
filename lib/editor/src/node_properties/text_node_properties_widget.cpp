#include "cathedral/editor/common/vertical_separator.hpp"
#include "cathedral/editor/node_properties/font_selector.hpp"
#include "cathedral/engine/scene.hpp"

#include <QLabel>
#include <QVBoxLayout>
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

        _main_layout->addWidget(new QLabel("Font"));
        _main_layout->addWidget(new vertical_separator(this));

        auto* font_selector = new editor::font_selector(_project, scene, this, "no font");
        _main_layout->addWidget(font_selector);

        connect(font_selector, &font_selector::font_selected, this, [this, text_node](const std::shared_ptr<project::font_asset>& asset) {
            text_node->set_font_name(asset->name());
        });

        _main_layout->addStretch(1);
    }
} // namespace cathedral::editor