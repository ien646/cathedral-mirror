#include <cathedral/editor/node_properties/mesh3d_properties_widget.hpp>

#include <cathedral/editor/common/path_selector.hpp>
#include <cathedral/editor/common/transform_widget.hpp>
#include <cathedral/editor/common/vertical_separator.hpp>

#include <cathedral/editor/node_properties/mesh_selector.hpp>
#include <cathedral/engine/nodes/mesh3d_node.hpp>
#include <cathedral/engine/scene.hpp>

#include <cathedral/project/project.hpp>

#include <QLabel>
#include <QVBoxLayout>

namespace cathedral::editor
{
    mesh3d_properties_widget::mesh3d_properties_widget(project::project* pro, QWidget* parent, engine::mesh3d_node* node)
        : QWidget(parent)
        , _project(pro)
        , _node(node)
    {
        _main_layout = new QVBoxLayout(this);
        _main_layout->setSpacing(4);
        setLayout(_main_layout);

        _transform_widget = new transform_widget(this);

        _mesh_selector =
            new mesh_selector(_project, this, node->mesh_name() ? QString::fromStdString(*node->mesh_name()) : "");

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

        connect(_mesh_selector, &mesh_selector::mesh_selected, this, [this](std::shared_ptr<project::mesh_asset> asset) {
            if (!asset)
            {
                return;
            }

            const auto mesh = asset->load_mesh();
            _node->set_mesh(asset->relative_path(), mesh);

            _mesh_selector->set_text(QString::fromStdString(_project->relpath_to_name(asset->relative_path())));
        });

        init_ui();
    }

    void mesh3d_properties_widget::init_ui()
    {
        _main_layout->addWidget(_transform_widget, 0, Qt::AlignTop);
        _main_layout->addWidget(new vertical_separator(this), 0);
        _main_layout->addWidget(new QLabel("Mesh"), 0, Qt::AlignmentFlag::AlignRight);
        _main_layout->addWidget(_mesh_selector, 0, Qt::AlignTop);
        _main_layout->addWidget(new vertical_separator(this), 0);
        _main_layout->addStretch(1);

        update_transform_widget();

        if (_node->mesh_name().has_value())
        {
            _mesh_selector->set_text(QString::fromStdString(*_node->mesh_name()));
        }
        else
        {
            _mesh_selector->set_text("__INLINE_MESH__");
        }
    }

    void mesh3d_properties_widget::update_transform_widget()
    {
        _transform_widget->set_position(_node->local_position());
        _transform_widget->set_rotation(_node->local_rotation());
        _transform_widget->set_scale(_node->local_scale());
    }
} // namespace cathedral::editor