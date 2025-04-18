#include "cathedral/bits/error.hpp"
#include <cathedral/editor/node_properties/camera2d_properties_widget.hpp>

#include <cathedral/editor/common/float_grid.hpp>
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
        _bounds_x = new float_grid({ 1U, 2U }, this);
        _bounds_y = new float_grid({ 1U, 2U }, this);
        _bounds_z = new float_grid({ 1U, 2U }, this);

        auto* bounds_widget = new QWidget(this);
        auto* bounds_layout = new QVBoxLayout(this);
        bounds_widget->setLayout(bounds_layout);
        bounds_layout->addWidget(new QLabel("[xmin, xmax]"), 0);
        bounds_layout->addWidget(_bounds_x, 1);
        bounds_layout->addWidget(new QLabel("[ymin, ymax]"), 0);
        bounds_layout->addWidget(_bounds_y, 1);
        bounds_layout->addWidget(new QLabel("[znear, zfar]"), 0);
        bounds_layout->addWidget(_bounds_z, 1);
        bounds_layout->addStretch();

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
        main_layout->addWidget(bounds_widget, 0, Qt::AlignTop);
        main_layout->addWidget(main_camera_checkbox);
        main_layout->addStretch();

        connect(_transform_widget, &transform_widget::position_changed, this, [this](glm::vec3 position) {
            _node->set_local_position(position);
            update_transform_widget();
        });

        connect(_transform_widget, &transform_widget::rotation_changed, this, [this](glm::vec3 rotation) {
            _node->set_local_rotation(rotation);
            update_transform_widget();
        });

        connect(_bounds_x, &float_grid::value_changed, this, [this](const std::vector<float>& values) {
            CRITICAL_CHECK(values.size() == 2, "Invalid float grid dimensions")
            auto& cam = _node->camera();
            cam.set_xmin(values[0]);
            cam.set_xmax(values[1]);
        });

        connect(_bounds_y, &float_grid::value_changed, this, [this](const std::vector<float>& values) {
            CRITICAL_CHECK(values.size() == 2, "Invalid float grid dimensions")
            auto& cam = _node->camera();
            cam.set_ymin(values[0]);
            cam.set_ymax(values[1]);
        });

        connect(_bounds_z, &float_grid::value_changed, this, [this](const std::vector<float>& values) {
            CRITICAL_CHECK(values.size() == 2, "Invalid float grid dimensions")
            auto& cam = _node->camera();
            cam.set_znear(values[0]);
            cam.set_zfar(values[1]);
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

    void camera2d_properties_widget::paintEvent(QPaintEvent* ev)
    {
        if (_node != nullptr)
        {
            update_transform_widget();
        }
        QWidget::paintEvent(ev);
    }

    void camera2d_properties_widget::init_ui()
    {
        const auto& cam = _node->camera();
        _bounds_x->set_value(0, 0, cam.xmin());
        _bounds_x->set_value(0, 1, cam.xmax());
        _bounds_y->set_value(0, 0, cam.ymin());
        _bounds_y->set_value(0, 1, cam.ymax());
        _bounds_z->set_value(0, 0, cam.znear());
        _bounds_z->set_value(0, 1, cam.zfar());
        update_transform_widget();
    }

    void camera2d_properties_widget::update_transform_widget()
    {
        _transform_widget->set_position(_node->local_position());
        _transform_widget->set_rotation(_node->local_rotation());
    }
} // namespace cathedral::editor