#include <cathedral/editor/node_properties/mesh3d_properties_widget.hpp>

namespace cathedral::editor
{
    mesh3d_properties_widget::mesh3d_properties_widget(QWidget* parent)
        : QWidget(parent)
    {
        _main_layout = new QVBoxLayout(this);
        _main_layout->setSpacing(2);
        setLayout(_main_layout);

        _transform_widget = new transform_widget(this);
        _mesh_selector = new path_selector(path_selector_mode::FILE, "Mesh", this);

        _main_layout->addWidget(_transform_widget, 0, Qt::AlignTop);
        _main_layout->addWidget(_mesh_selector, 0, Qt::AlignTop);

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

        connect(_mesh_selector, &path_selector::paths_selected, this, [this](QStringList paths) {
            if (!paths.empty())
            {
                _node->set_mesh(paths[0].toStdString());
            }
        });
    }

    void mesh3d_properties_widget::set_node(engine::mesh3d_node* node)
    {
        _node = node;
        update_transform_widget();
    }

    void mesh3d_properties_widget::paintEvent(QPaintEvent* ev)
    {
        if (_node)
        {
            update_transform_widget();
        }
        QWidget::paintEvent(ev);
    }

    void mesh3d_properties_widget::update_transform_widget()
    {
        _transform_widget->set_position(_node->local_position());
        _transform_widget->set_rotation(_node->local_rotation());
        _transform_widget->set_scale(_node->local_scale());
    }
} // namespace cathedral::editor