#include "cathedral/editor/common/float_edit.hpp"
#include "cathedral/editor/common/sliding_float.hpp"
#include "cathedral/editor/common/vector3_widget.hpp"

#include <QComboBox>
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

        if (text_node->font_name().has_value())
        {
            font_selector->set_text(QSTR(*text_node->font_name()));
        }

        connect(
            font_selector,
            &font_selector::font_selected,
            this,
            [text_node, font_selector](const std::shared_ptr<project::font_asset>& asset) {
                if (asset == nullptr)
                {
                    text_node->set_font_name({});
                    font_selector->set_text("No font");
                }
                else
                {
                    text_node->set_font_name(asset->name());
                    font_selector->set_text(QSTR(asset->name()));
                }
            });

        auto* mode_layout = new QHBoxLayout;
        mode_layout->addWidget(new QLabel("Mode: "));

        auto* mode_combo = new QComboBox;
        for (const auto& entry : magic_enum::enum_names<engine::text_node_font_mode>())
        {
            mode_combo->addItem(QSTR(entry));
        }
        mode_combo->setCurrentText(QSTR(magic_enum::enum_name(text_node->mode())));
        mode_layout->addWidget(mode_combo);

        connect(mode_combo, &QComboBox::currentTextChanged, this, [text_node](const QString& text) {
            const auto mode_opt = magic_enum::enum_cast<engine::text_node_font_mode>(text.toStdString());
            CRITICAL_CHECK(mode_opt.has_value(), "Invalid font mode enum");
            text_node->set_mode(*mode_opt);
        });

        _main_layout->addLayout(mode_layout);

        _main_layout->addWidget(new QLabel("Text"));
        _main_layout->addWidget(new vertical_separator(this));

        auto* text_layout = new QHBoxLayout;
        _main_layout->addLayout(text_layout);
        _main_layout->addSpacing(4);

        text_layout->addWidget(new QLabel("Text:  "));

        auto* text_edit = new QLineEdit;
        text_edit->setText(QString::fromStdU32String(text_node->text()));
        text_layout->addWidget(text_edit, 1);

        connect(text_edit, &QLineEdit::textChanged, this, [text_node](const QString& text) {
            text_node->set_text(text.toStdU32String());
        });

        auto* color_layout = new QHBoxLayout;
        _main_layout->addLayout(color_layout);
        _main_layout->addSpacing(4);

        color_layout->addWidget(new QLabel("Color: "));

        auto* color_edit = new vector3_widget(this);
        color_layout->addWidget(color_edit);

        const auto init_color = text_node->text_color();
        color_edit->set_value(init_color.x, init_color.y, init_color.z);

        connect(color_edit, &vector3_widget::value_changed, this, [text_node](float x, float y, float z) {
            text_node->set_text_color({ x, y, z });
        });

        auto* hspacing_layout = new QHBoxLayout;
        _main_layout->addLayout(hspacing_layout);
        _main_layout->addSpacing(4);

        auto* hspacing_edit = new sliding_float(this, "Horizontal Spacing");
        hspacing_layout->addWidget(hspacing_edit);

        hspacing_edit->set_value(text_node->horizontal_spacing());

        connect(hspacing_edit, &sliding_float::value_changed, this, [text_node](const float& value) {
            text_node->set_horizontal_spacing(value);
        });

        _main_layout->addStretch(1);
    }
} // namespace cathedral::editor