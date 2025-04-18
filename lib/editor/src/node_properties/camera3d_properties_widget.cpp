#include <cathedral/editor/node_properties/camera3d_properties_widget.hpp>

#include <cathedral/engine/nodes/camera3d_node.hpp>

#include <cathedral/editor/common/sliding_float.hpp>
#include <cathedral/editor/common/transform_widget.hpp>
#include <cathedral/editor/common/vertical_separator.hpp>

#include <QCheckBox>
#include <QLabel>
#include <QVBoxLayout>

namespace cathedral::editor
{
    camera3d_properties_widget::camera3d_properties_widget(QWidget* parent, engine::camera3d_node* node)
        : QWidget(parent)
        , _node(node)
    {
        _main_layout = new QVBoxLayout(this);
        _main_layout->setSpacing(2);
        setLayout(_main_layout);

        _transform_widget = new transform_widget(this, true);
        _fov_slider = new sliding_float(this);
        _fov_slider->set_label("Vertical FOV");
        _fov_slider->set_label_color(QColor(128, 128, 128));
        _fov_slider->set_step(0.1F);

        auto* fov_widget = new QWidget(this);
        auto* fov_layout = new QHBoxLayout(this);
        fov_widget->setLayout(fov_layout);
        fov_layout->addWidget(_fov_slider);
        fov_layout->addStretch();

        auto* main_camera_checkbox = new QCheckBox;
        main_camera_checkbox->setText("Main camera");
        if (_node->is_main_camera())
        {
            main_camera_checkbox->setCheckState(Qt::CheckState::Checked);
        }

        auto* transform_label = new QLabel("<u>Transform</u>");
        transform_label->setTextFormat(Qt::TextFormat::RichText);

        _main_layout->addWidget(transform_label, 0, Qt::AlignmentFlag::AlignRight);
        _main_layout->addWidget(_transform_widget, 0, Qt::AlignTop);
        _main_layout->addWidget(new vertical_separator(this), 0, Qt::AlignTop);
        _main_layout->addWidget(fov_widget, 0, Qt::AlignTop);
        _main_layout->addWidget(main_camera_checkbox);
        _main_layout->addStretch();

        connect(_transform_widget, &transform_widget::position_changed, this, [this](glm::vec3 position) {
            _node->set_local_position(position);
            update_transform_widget();
        });

        connect(_transform_widget, &transform_widget::rotation_changed, this, [this](glm::vec3 rotation) {
            _node->set_local_rotation(rotation);
            update_transform_widget();
        });

        connect(_fov_slider, &sliding_float::value_changed, this, [this](float value) {
            _node->camera().set_vertical_fov(value);
        });

        connect(main_camera_checkbox, &QCheckBox::checkStateChanged, this, [this](Qt::CheckState state) {
            if (state == Qt::CheckState::Checked)
            {
                _node->set_main_camera(true);
            }
            else if (state == Qt::CheckState::Unchecked)
            {
                _node->set_main_camera(false);
            }
        });

        init_ui();
    }

    void camera3d_properties_widget::init_ui()
    {
        _fov_slider->set_value(_node->camera().vertical_fov());
        update_transform_widget();
    }

    void camera3d_properties_widget::paintEvent(QPaintEvent* ev)
    {
        if (_node != nullptr)
        {
            update_transform_widget();
        }
        QWidget::paintEvent(ev);
    }

    void camera3d_properties_widget::update_transform_widget()
    {
        _transform_widget->set_position(_node->local_position());
        _transform_widget->set_rotation(_node->local_rotation());
    }
} // namespace cathedral::editor