#include <cathedral/editor/node_properties/camera2d_properties_widget.hpp>

#include <cathedral/editor/common/float_grid.hpp>
#include <cathedral/editor/common/sliding_float.hpp>
#include <cathedral/editor/common/vertical_separator.hpp>

#include <QCheckBox>
#include <QLabel>
#include <QVBoxLayout>

namespace cathedral::editor
{
    camera2d_properties_widget::camera2d_properties_widget(QWidget* parent, engine::camera2d_node* node)
        : QWidget(parent)
        , _node(node)
    {
        auto* main_layout = new QVBoxLayout(this);
        main_layout->setSpacing(2);
        setLayout(main_layout);

        _transform_widget = new transform_widget(this, true);

        auto* znear_layout = new QHBoxLayout(this);
        _znear_slider = new sliding_float(this, "      near-z");
        _znear_slider->set_label_color(QColor(128, 128, 128));
        znear_layout->addWidget(_znear_slider, 0);
        znear_layout->addStretch();

        auto* zfar_layout = new QHBoxLayout(this);
        _zfar_slider = new sliding_float(this, "       far-z");
        _zfar_slider->set_label_color(QColor(128, 128, 128));
        zfar_layout->addWidget(_zfar_slider);
        zfar_layout->addStretch();

        auto* main_camera_checkbox = new QCheckBox;
        main_camera_checkbox->setText("Main camera");
        if (_node->is_main_camera())
        {
            main_camera_checkbox->setCheckState(Qt::CheckState::Checked);
        }

        auto* transform_label = new QLabel("<u>Transform</u>");
        transform_label->setTextFormat(Qt::TextFormat::RichText);

        main_layout->addWidget(transform_label, 0, Qt::AlignmentFlag::AlignRight);
        main_layout->addWidget(_transform_widget, 0, Qt::AlignTop);
        main_layout->addWidget(new vertical_separator(this), 0, Qt::AlignTop);
        main_layout->addLayout(znear_layout, 0);
        main_layout->addLayout(zfar_layout, 0);
        main_layout->addWidget(main_camera_checkbox);
        main_layout->addStretch();

        connect(_transform_widget, &transform_widget::position_changed, this, [this](const glm::vec3 position) {
            _node->set_local_position(position);
            update_ui();
        });

        connect(_transform_widget, &transform_widget::rotation_changed, this, [this](const glm::vec3 rotation) {
            _node->set_local_rotation(rotation);
            update_ui();
        });

        connect(_znear_slider, &sliding_float::value_changed, this, [this](float value) {
            _node->camera().set_near_z(value);
            update_ui();
        });

        connect(_zfar_slider, &sliding_float::value_changed, this, [this](float value) {
            _node->camera().set_far_z(value);
            update_ui();
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

        update_ui();
    }

    void camera2d_properties_widget::paintEvent(QPaintEvent* ev)
    {
        if (_node != nullptr)
        {
            update_transform_widget();
        }
        QWidget::paintEvent(ev);
    }

    void camera2d_properties_widget::update_ui() const
    {
        const auto& cam = _node->camera();
        _znear_slider->set_value(cam.near_z());
        _zfar_slider->set_value(cam.far_z());
        update_transform_widget();
    }

    void camera2d_properties_widget::update_transform_widget() const
    {
        _transform_widget->set_position(_node->local_position());
        _transform_widget->set_rotation(_node->local_rotation());
    }
} // namespace cathedral::editor