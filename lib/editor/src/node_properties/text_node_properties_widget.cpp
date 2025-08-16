#include <cathedral/editor/node_properties/text_node_properties_widget.hpp>

#include <cathedral/editor/common/vertical_separator.hpp>
#include <cathedral/editor/node_properties/font_selector.hpp>
#include <cathedral/editor/node_properties/material_selector.hpp>
#include <cathedral/editor/utils.hpp>
#include <cathedral/engine/scene.hpp>

#include <QLabel>
#include <QLineEdit>
#include <QVBoxLayout>

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

        connect(
            font_selector,
            &font_selector::font_selected,
            this,
            [text_node, font_selector](const std::shared_ptr<project::font_asset>& asset) {
                text_node->set_font_name(asset->name());
                font_selector->set_text(QSTR(asset->name()));
            });

        _main_layout->addWidget(new QLabel("Material"));
        _main_layout->addWidget(new vertical_separator(this));

        auto* material_selector = new editor::material_selector(_project, scene, this, "no material");
        _main_layout->addWidget(material_selector);

        connect(
            material_selector,
            &material_selector::material_selected,
            this,
            [text_node, material_selector](const std::shared_ptr<project::material_asset>& asset) {
                if (asset == nullptr)
                {
                    return;
                }
                text_node->set_material(asset->name());
                material_selector->set_text(QSTR(asset->name()));
            });

        _main_layout->addWidget(new QLabel("Text"));
        _main_layout->addWidget(new vertical_separator(this));

        auto* text_layout = new QHBoxLayout;
        _main_layout->addLayout(text_layout);

        text_layout->addWidget(new QLabel("Text: "));

        auto* text_edit = new QLineEdit;
        text_edit->setText("Lorem Ipsum");
        text_layout->addWidget(text_edit, 1);

        connect(text_edit, &QLineEdit::textChanged, this, [text_node](const QString& text) {
            text_node->set_text(text.toStdU32String());
        });

        _main_layout->addStretch(1);
    }
} // namespace cathedral::editor