#include <cathedral/editor/node_properties/node_properties_widget.hpp>

#include <cathedral/editor/common/path_selector.hpp>
#include <cathedral/editor/common/transform_widget.hpp>
#include <cathedral/editor/common/vertical_separator.hpp>

#include <cathedral/engine/nodes/node.hpp>
#include <cathedral/engine/scene.hpp>

#include <cathedral/project/project.hpp>

#include <QLabel>
#include <QVBoxLayout>

namespace cathedral::editor
{
    node_properties_widget::node_properties_widget(project::project* pro, QWidget* parent, engine::node* node)
        : QWidget(parent)
        , _project(pro)
        , _node(node)
    {
        _main_layout = new QVBoxLayout(this);
        _main_layout->setSpacing(4);
        setLayout(_main_layout);

        _transform_widget = new transform_widget(this);

        connect(_transform_widget, &transform_widget::position_changed, this, [this](glm::vec3 position) {
            _node->set_local_position(position);
            update_transform_widget();
        });

        connect(_transform_widget, &transform_widget::rotation_changed, this, [this](glm::vec3 rotation) {
            _node->set_local_rotation(rotation);
            update_transform_widget();
        });

        connect(_transform_widget, &transform_widget::scale_changed, this, [this](glm::vec3 scale) {
            _node->set_local_scale(scale);
            update_transform_widget();
        });

        init_ui();
    }

    void node_properties_widget::init_ui()
    {
        auto* transform_label = new QLabel("<u>Transform</u>");
        transform_label->setTextFormat(Qt::TextFormat::RichText);

        _main_layout->addWidget(transform_label, 0, Qt::AlignmentFlag::AlignRight);
        _main_layout->addWidget(_transform_widget, 0, Qt::AlignTop);
        _main_layout->addStretch(1);

        update_transform_widget();
    }

    void node_properties_widget::update_transform_widget() const
    {
        _transform_widget->set_position(_node->local_position());
        _transform_widget->set_rotation(_node->local_rotation());
        _transform_widget->set_scale(_node->local_scale());
    }
} // namespace cathedral::editor