#include <cathedral/editor/node_properties/mesh3d_properties_widget.hpp>

#include <cathedral/editor/common/path_selector.hpp>
#include <cathedral/editor/common/transform_widget.hpp>
#include <cathedral/editor/common/vertical_separator.hpp>

#include <cathedral/editor/node_properties/material_selector.hpp>
#include <cathedral/editor/node_properties/mesh_selector.hpp>
#include <cathedral/editor/node_properties/texture_selector.hpp>

#include <cathedral/editor/utils.hpp>

#include <cathedral/engine/nodes/mesh3d_node.hpp>
#include <cathedral/engine/scene.hpp>

#include <cathedral/project/project.hpp>

#include <QLabel>
#include <QTimer>
#include <QVBoxLayout>
#include <utility>

namespace cathedral::editor
{
    mesh3d_properties_widget::mesh3d_properties_widget(
        project::project* pro,
        std::shared_ptr<engine::scene> scene,
        QWidget* parent,
        engine::mesh3d_node* node)
        : QWidget(parent)
        , _project(pro)
        , _scene(std::move(scene))
        , _node(node)
    {
        CRITICAL_CHECK_NOTNULL(_node);

        _main_layout = new QVBoxLayout(this);
        _main_layout->setSpacing(4);
        setLayout(_main_layout);

        _transform_widget = new transform_widget(this);

        _mesh_selector = new mesh_selector(_project, this, QSTR(_node->mesh_name().has_value() ? _node->mesh_name().value() : ""));

        const auto node_material = _node->get_material();
        _material_selector = new material_selector(
            _project,
            _scene,
            this,
            (node_material.expired()) ? "" : QSTR(node_material.lock()->name()));

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

        connect(
            _mesh_selector,
            &mesh_selector::mesh_selected,
            this,
            [this](const std::shared_ptr<project::mesh_asset>& asset) {
            if (!asset)
            {
                return;
            }

            _node->set_mesh(asset->name());
            _mesh_selector->set_text(QSTR(asset->name()));
            });

        connect(
            _material_selector,
            &material_selector::material_selected,
            this,
            [this](const std::shared_ptr<project::material_asset>& asset) {
            if (!asset)
            {
                refresh_node_texture_selectors();
                return;
            }

            _node->set_material(asset->name());
            _material_selector->set_text(QSTR(asset->name()));

            QTimer::singleShot(200, Qt::TimerType::CoarseTimer, [this] { refresh_node_texture_selectors(); });
            });

        init_ui();
    }

    void mesh3d_properties_widget::init_ui()
    {
        auto* transform_label = new QLabel("<u>Transform</u>");
        transform_label->setTextFormat(Qt::TextFormat::RichText);

        auto* mesh_label = new QLabel("<u>Mesh</u>");
        mesh_label->setTextFormat(Qt::TextFormat::RichText);

        auto* material_label = new QLabel("<u>Material</u>");
        material_label->setTextFormat(Qt::TextFormat::RichText);

        _stretch = new QWidget(this);

        _main_layout->addWidget(transform_label, 0, Qt::AlignmentFlag::AlignRight);
        _main_layout->addWidget(_transform_widget, 0, Qt::AlignTop);
        _main_layout->addWidget(new vertical_separator(this), 0);
        _main_layout->addWidget(mesh_label, 0, Qt::AlignmentFlag::AlignRight);
        _main_layout->addWidget(_mesh_selector, 0, Qt::AlignTop);
        _main_layout->addWidget(new vertical_separator(this), 0);
        _main_layout->addWidget(material_label, 0, Qt::AlignmentFlag::AlignRight);
        _main_layout->addWidget(_material_selector, 0, Qt::AlignTop);
        _main_layout->addWidget(new vertical_separator(this), 0);
        _main_layout->addWidget(_stretch, 1);

        update_transform_widget();

        if (_node->mesh_name().has_value())
        {
            _mesh_selector->set_text(QSTR(_node->mesh_name().has_value() ? _node->mesh_name().value() : ""));
        }
        else
        {
            _mesh_selector->set_text("__INLINE_MESH__");
        }

        refresh_node_texture_selectors();
    }

    void mesh3d_properties_widget::update_transform_widget()
    {
        _transform_widget->set_position(_node->local_position());
        _transform_widget->set_rotation(_node->local_rotation());
        _transform_widget->set_scale(_node->local_scale());
    }

    void mesh3d_properties_widget::refresh_node_texture_selectors()
    {
        delete _node_textures_layout;
        _node_textures_layout = new QVBoxLayout(this);
        _node_textures_layout->setAlignment(Qt::AlignmentFlag::AlignTop);
        _node_textures_layout->setContentsMargins(0, 0, 0, 0);

        if (_node->get_material().expired() || _node->get_material().lock()->node_texture_slots() == 0)
        {
            return;
        }

        _main_layout->removeWidget(_stretch);

        auto* node_textures_label = new QLabel("<u>Node textures</u>");
        node_textures_label->setTextFormat(Qt::TextFormat::RichText);

        _main_layout->addWidget(node_textures_label, 0, Qt::AlignmentFlag::AlignRight);
        _main_layout->addLayout(_node_textures_layout, 0);
        _main_layout->addWidget(_stretch, 1);

        const auto& material = _node->get_material();
        for (size_t i = 0; i < material.lock()->node_texture_slots(); ++i)
        {
            const auto& bound_texture = _node->bound_textures()[i];

            auto* selector = new texture_selector(
                _project,
                this,
                QSTR("Slot {}: {}", i, bound_texture ? bound_texture->name() : "Default"));
            _main_layout->addWidget(selector, 0, Qt::AlignTop);

            connect(
                selector,
                &texture_selector::texture_selected,
                this,
                [this, i, selector](const std::shared_ptr<project::texture_asset>& texture_asset) {
                    if (texture_asset == nullptr)
                    {
                        return;
                    }

                    selector->set_text(QSTR(texture_asset->name()));
                    _node->bind_node_texture_slot(texture_asset->name(), i);
                });
        }
    }
} // namespace cathedral::editor