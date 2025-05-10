#include "cathedral/editor/common/sliding_float.hpp"

#include <cathedral/editor/node_properties/point_light_properties_widget.hpp>

#include <cathedral/editor/common/transform_widget.hpp>
#include <cathedral/engine/nodes/point_light_node.hpp>

#include <QLabel>
#include <QVBoxLayout>

namespace cathedral::editor
{
    constexpr auto INTENSITY_LABEL = "Intensity";
    constexpr auto RANGE_LABEL = "    Range";
    constexpr auto FALLOFF_LABEL = "  Falloff";

    point_light_properties_widget::point_light_properties_widget(
        project::project* pro,
        std::shared_ptr<engine::scene> scene,
        QWidget* parent,
        engine::point_light_node* node)
        : node_properties_widget(pro, parent, static_cast<engine::node*>(node), false)
        , _node(node)
    {
        CRITICAL_CHECK_NOTNULL(_project);
        CRITICAL_CHECK_NOTNULL(_node);

        _main_layout->addSpacing(8);

        auto* intensity = new sliding_float(this, INTENSITY_LABEL);
        intensity->set_value(_node->intensity());
        _main_layout->addWidget(intensity);

        auto* range = new sliding_float(this, RANGE_LABEL);
        range->set_value(_node->range());
        _main_layout->addWidget(range);

        auto* falloff = new sliding_float(this, FALLOFF_LABEL);
        falloff->set_value(_node->falloff_coefficient());
        _main_layout->addWidget(falloff);

        _main_layout->addWidget(new QLabel("Color"));

        auto* color_r = new sliding_float(this, "R");
        color_r->set_label_color({ 128, 0, 0 });
        color_r->set_range(0, 1);
        color_r->set_step(0.001F);
        color_r->set_value(_node->color().r);

        auto* color_g = new sliding_float(this, "G");
        color_g->set_label_color({ 0, 128, 0 });
        color_g->set_range(0, 1);
        color_g->set_step(0.001F);
        color_g->set_value(_node->color().g);

        auto* color_b = new sliding_float(this, "B");
        color_b->set_label_color({ 0, 0, 128 });
        color_b->set_range(0, 1);
        color_b->set_step(0.001F);
        color_b->set_value(_node->color().b);

        auto* color_preview_label = new QLabel("      ");

        auto* color_layout = new QHBoxLayout(this);
        color_layout->addWidget(color_r, 2);
        color_layout->addWidget(color_g, 2);
        color_layout->addWidget(color_b, 2);
        color_layout->addWidget(new QLabel("=>"));
        color_layout->addWidget(color_preview_label, 1);

        _main_layout->addLayout(color_layout);

        _main_layout->addStretch(1);

        const auto update_color_preview_label = [this, color_preview_label] {
            const auto color = _node->color();
            const auto stylesheet = QString{ "QLabel{ background-color: rgba(%1, %2, %3, 255); }" }
                                        .arg(color.r * 255)
                                        .arg(color.g * 255)
                                        .arg(color.b * 255);
            color_preview_label->setStyleSheet(stylesheet);
        };

        connect(intensity, &sliding_float::value_changed, this, [this](const float value) { _node->set_insensity(value); });

        connect(range, &sliding_float::value_changed, this, [this](const float value) { _node->set_range(value); });

        connect(falloff, &sliding_float::value_changed, this, [this](const float value) {
            _node->set_falloff_coefficient(value);
        });

        connect(color_r, &sliding_float::value_changed, this, [this, update_color_preview_label](const float value) {
            auto color = _node->color();
            color.r = value;
            _node->set_color(color);
            update_color_preview_label();
        });

        connect(color_g, &sliding_float::value_changed, this, [this, update_color_preview_label](const float value) {
            auto color = _node->color();
            color.g = value;
            _node->set_color(color);
            update_color_preview_label();
        });

        connect(color_b, &sliding_float::value_changed, this, [this, update_color_preview_label](const float value) {
            auto color = _node->color();
            color.b = value;
            _node->set_color(color);
            update_color_preview_label();
        });

        connect(_transform_widget, &transform_widget::position_changed, this, [this](const glm::vec3 position) {
            _node->set_position(position);
        });

        update_color_preview_label();
    }
} // namespace cathedral::editor