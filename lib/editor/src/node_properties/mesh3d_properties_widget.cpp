#include "cathedral/editor/common/shader_variable_selector.hpp"

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
#include <QLayoutItem>
#include <QTimer>
#include <QVBoxLayout>
#include <utility>

namespace cathedral::editor
{
    mesh3d_properties_widget::mesh3d_properties_widget(
        project::project* pro,
        engine::scene& scene,
        QWidget* parent,
        engine::mesh3d_node* node)
        : QWidget(parent)
        , _project(pro)
        , _scene(scene)
        , _node(node)
    {
        CRITICAL_CHECK_NOTNULL(_node);

        setObjectName("mesh3d_properties_widget");

        _main_layout = new QVBoxLayout(this);
        _main_layout->setSpacing(4);
        setLayout(_main_layout);

        _node_properties_widget = new node_properties_widget(_project, this, _node, false);

        _transform_update_timer = new QTimer(this);
        _transform_update_timer->setInterval(100);
        _transform_update_timer->start();

        _mesh_selector =
            new mesh_selector(_project, _scene, this, QSTR(_node->mesh_name().has_value() ? _node->mesh_name().value() : ""));

        const auto node_material = _node->get_material();
        _material_selector =
            new material_selector(_project, _scene, this, node_material.expired() ? "" : QSTR(node_material.lock()->name()));

        _node_textures_label = new QLabel("<u>Node textures</u>");
        _node_textures_label->setTextFormat(Qt::TextFormat::RichText);

        _node_variables_label = new QLabel("<u>Node variables</u>");
        _node_variables_label->setTextFormat(Qt::TextFormat::RichText);

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
                    refresh_node_variable_widgets();
                    return;
                }

                _node->set_material(asset->name());
                _material_selector->set_text(QSTR(asset->name()));

                QTimer::singleShot(200, Qt::TimerType::CoarseTimer, [this] {
                    refresh_node_texture_selectors();
                    refresh_node_variable_widgets();
                });
            });

        connect(_transform_update_timer, &QTimer::timeout, this, [this] { update_transform_widget(); });

        init_ui();
    }

    void mesh3d_properties_widget::init_ui()
    {
        auto* mesh_label = new QLabel("<u>Mesh</u>");
        mesh_label->setTextFormat(Qt::TextFormat::RichText);

        auto* material_label = new QLabel("<u>Material</u>");
        material_label->setTextFormat(Qt::TextFormat::RichText);

        _stretch = new QWidget(this);

        _main_layout->addWidget(_node_properties_widget, 0, Qt::AlignTop);
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
        refresh_node_variable_widgets();
    }

    void mesh3d_properties_widget::update_transform_widget() const
    {
        _node_properties_widget->update_transform_widget();
    }

    void mesh3d_properties_widget::refresh_node_texture_selectors()
    {
        if (_node_textures_layout != nullptr)
        {
            while (const auto* child = _node_textures_layout->takeAt(0))
            {
                child->widget()->deleteLater();
                delete child;
            }
            delete _node_textures_layout;
        }
        _node_textures_layout = new QVBoxLayout(this);
        _node_textures_layout->setAlignment(Qt::AlignmentFlag::AlignTop);
        _node_textures_layout->setContentsMargins(0, 0, 0, 0);

        if (_node->get_material().expired() || _node->get_material().lock()->node_texture_slots() == 0)
        {
            return;
        }

        _main_layout->removeWidget(_stretch);
        _main_layout->removeWidget(_node_textures_label);

        _main_layout->addWidget(_node_textures_label, 0, Qt::AlignmentFlag::AlignRight);
        _main_layout->addLayout(_node_textures_layout, 0);
        _main_layout->addWidget(_stretch, 1);

        const auto& material = _node->get_material();
        for (size_t i = 0; i < material.lock()->node_texture_slots(); ++i)
        {
            const auto& bound_texture = _node->texture_names()[i];

            auto* selector = new texture_selector(_project, _scene, this, QSTR("Slot {}: {}", i, bound_texture));
            _node_textures_layout->addWidget(selector, 0, Qt::AlignTop);

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
                    _node->bind_node_texture_slot(texture_asset->name(), static_cast<uint32_t>(i));
                });
        }
    }

    void mesh3d_properties_widget::refresh_node_variable_widgets()
    {
        if (_node_variables_layout != nullptr)
        {
            while (const auto* child = _node_variables_layout->takeAt(0))
            {
                child->widget()->deleteLater();
                delete child;
            }
            delete _node_variables_layout;
        }
        _node_variables_layout = new QGridLayout(this);
        _node_variables_layout->setAlignment(Qt::AlignmentFlag::AlignTop);
        _node_variables_layout->setContentsMargins(0, 0, 0, 0);

        if (_node->get_material().expired() || _node->get_material().lock()->node_texture_slots() == 0)
        {
            return;
        }

        _main_layout->removeWidget(_stretch);
        _main_layout->removeWidget(_node_variables_label);

        _main_layout->addWidget(new vertical_separator(this));
        _main_layout->addWidget(_node_variables_label, 0, Qt::AlignmentFlag::AlignRight);
        _main_layout->addLayout(_node_variables_layout, 0);
        _main_layout->addWidget(_stretch, 1);

        const auto& material = _node->get_material();
        for (size_t i = 0; i < material.lock()->node_variables().size(); ++i)
        {
            const auto& node_var = material.lock()->node_variables()[i];

            const auto node_bindings_range = material.lock()->node_bindings() | std::views::values;

            // If variable is bound, skip widget creation
            auto it = std::ranges::find(node_bindings_range, node_var.name);
            if (it != node_bindings_range.end())
            {
                continue;
            }

            auto* var_widget = new shader_variable_selector(node_var.type, this);
            var_widget->set_value(_node->get_node_variable_value(node_var.name, node_var.type));

            connect(
                var_widget,
                &shader_variable_selector::value_changed,
                this,
                [this, name = node_var.name](const gfx::shader_data_value& value) {
                    std::visit([&](const auto& vval) { _node->set_node_variable_value(name, vval); }, value);
                });
            _node_variables_layout->addWidget(new QLabel(QSTR(node_var.name)), static_cast<int>(i), 0, Qt::AlignLeft);
            _node_variables_layout->addWidget(var_widget, static_cast<int>(i), 1, Qt::AlignLeft);
        }
    }
} // namespace cathedral::editor