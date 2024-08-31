#include <cathedral/editor/node_properties/mesh3d_properties_widget.hpp>

#include <cathedral/editor/common/vertical_separator.hpp>

#include <cathedral/engine/scene.hpp>

namespace cathedral::editor
{
    mesh3d_properties_widget::mesh3d_properties_widget(QWidget* parent, engine::mesh3d_node* node)
        : QWidget(parent)
        , _node(node)
    {
        _main_layout = new QVBoxLayout(this);
        _main_layout->setSpacing(4);
        setLayout(_main_layout);

        _transform_widget = new transform_widget(this);
        _mesh_selector = new path_selector(path_selector_mode::FILE, "Mesh", this);

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
                if (_node->mesh_name())
                {
                    _mesh_selector->set_text(QString::fromStdString(*_node->mesh_name()));
                }
                else
                {
                    _mesh_selector->set_text("__INLINE_MESH__");
                }
            }
        });

        init_ui();
    }

    void mesh3d_properties_widget::init_ui()
    {
        _main_layout->addWidget(_transform_widget, 0, Qt::AlignTop);
        _main_layout->addWidget(new vertical_separator(this));
        _main_layout->addWidget(_mesh_selector, 0, Qt::AlignTop);
        _main_layout->addWidget(new vertical_separator(this));

        update_transform_widget();
        init_texture_selectors();

        if (_node->mesh_name())
        {
            _mesh_selector->set_text(QString::fromStdString(*_node->mesh_name()));
        }
        else
        {
            _mesh_selector->set_text("__INLINE_MESH__");
        }
    }

    void mesh3d_properties_widget::paintEvent(QPaintEvent* ev)
    {
        if (_node)
        {
            update_transform_widget();
            for (size_t i = 0; i < _material_texture_selectors.size(); ++i)
            {
                const auto& tex = _node->get_material()->bound_textures()[i];
                if(tex->path())
                {
                    _material_texture_selectors[i]->set_text(QString::fromStdString(*tex->path()));
                }
            }

            for (size_t i = 0; i < _node_texture_selectors.size(); ++i)
            {
                const auto& tex = _node->bound_textures()[i];
                if(tex->path())
                {
                    _node_texture_selectors[i]->set_text(QString::fromStdString(*tex->path()));
                }
            }
        }
        QWidget::paintEvent(ev);
    }

    void mesh3d_properties_widget::update_transform_widget()
    {
        _transform_widget->set_position(_node->local_position());
        _transform_widget->set_rotation(_node->local_rotation());
        _transform_widget->set_scale(_node->local_scale());
    }

    void mesh3d_properties_widget::init_texture_selectors()
    {
        _main_layout->addWidget(new QLabel("Material texture slots:", this));
        for (size_t i = 0; i < _node->get_material()->definition().material_texture_slot_count(); ++i)
        {
            const QString label_text = "[" + QString::number(i) + "]";
            auto* selector = new path_selector(path_selector_mode::FILE, label_text, this);
            connect(selector, &path_selector::paths_selected, this, [this, i, selector](const QStringList paths) {
                if (paths.empty())
                {
                    return;
                }
                const auto image_path = paths[0];

                try
                {
                    const auto tex =
                        _node->get_scene().get_renderer().create_color_texture(ien::image(image_path.toStdString()));
                    _node->get_material()->bind_material_texture_slot(tex, i);
                    selector->set_text(image_path);
                }
                catch (const std::exception&)
                {
                    _node->get_material()->bind_material_texture_slot(_node->get_scene().get_renderer().default_texture(), i);
                    selector->set_text("__DEFAULT_TEXTURE__");
                }
            });
            _main_layout->addWidget(selector);
            _material_texture_selectors.push_back(selector);
        }

        if (_node->get_material()->definition().node_texture_slot_count() > 0)
        {
            _main_layout->addWidget(new vertical_separator(this));
            _main_layout->addWidget(new QLabel("Node texture slots:", this));
            for (size_t i = 0; i < _node->get_material()->definition().node_texture_slot_count(); ++i)
            {
                const QString label_text = "[" + QString::number(i) + "]";
                auto* selector = new path_selector(path_selector_mode::FILE, label_text, this);
                connect(selector, &path_selector::paths_selected, this, [this, i](const QStringList paths) {
                    if (paths.empty())
                    {
                        return;
                    }
                    const auto image_path = paths[0];

                    try
                    {
                        const auto tex = _node->get_scene().get_renderer().create_color_texture(
                            ien::image(image_path.toStdString()));
                        _node->bind_node_texture_slot(tex, i);
                    }
                    catch (const std::exception&)
                    {
                        _node->bind_node_texture_slot(_node->get_scene().get_renderer().default_texture(), i);
                    }
                });
                _main_layout->addWidget(selector);
                _node_texture_selectors.push_back(selector);
            }
        }

        _main_layout->addStretch();
    }
} // namespace cathedral::editor