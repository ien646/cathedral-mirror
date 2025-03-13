#include <cathedral/editor/common/transform_widget.hpp>

#include <cathedral/editor/common/vector3_widget.hpp>
#include <cathedral/editor/common/vertical_separator.hpp>

#include <QLabel>
#include <QVBoxLayout>

namespace cathedral::editor
{
    transform_widget::transform_widget(QWidget* parent, bool disable_scale)
        : QWidget(parent)
    {
        setSizePolicy(QSizePolicy::Policy::Expanding, QSizePolicy::Policy::Fixed);
        setMinimumSize(0, 0);
        setContentsMargins(0, 0, 0, 0);

        _main_layout = new QVBoxLayout(this);
        _main_layout->setSpacing(0);
        _main_layout->setContentsMargins(0, 0, 0, 0);

        _position_widget = new vector3_widget(this);
        _rotation_widget = new vector3_widget(this);
        if (!disable_scale)
        {
            _scale_widget = new vector3_widget(this);
        }
        _rotation_widget->set_step(0.1F);

        _main_layout->addWidget(new QLabel("Position", this), 0, Qt::AlignLeft);
        _main_layout->addWidget(_position_widget, 0, Qt::AlignTop | Qt::AlignLeft);
        _main_layout->addWidget(new QLabel("Rotation", this), 0, Qt::AlignLeft);
        _main_layout->addWidget(_rotation_widget, 0, Qt::AlignTop | Qt::AlignLeft);

        if (!disable_scale)
        {
            _main_layout->addWidget(new QLabel("Scale", this), 0, Qt::AlignLeft);
            _main_layout->addWidget(_scale_widget, 0, Qt::AlignTop | Qt::AlignLeft);
        }

        adjustSize();

        connect(_position_widget, &vector3_widget::value_changed, this, [this](float x, float y, float z) {
            emit position_changed({ x, y, z });
        });

        connect(_rotation_widget, &vector3_widget::value_changed, this, [this](float x, float y, float z) {
            emit rotation_changed({ x, y, z });
        });

        if (!disable_scale)
        {
            connect(_scale_widget, &vector3_widget::value_changed, this, [this](float x, float y, float z) {
                emit scale_changed({ x, y, z });
            });
        }
    }

    void transform_widget::set_position(float x, float y, float z)
    {
        _position_widget->set_value(x, y, z);
    }

    void transform_widget::set_position(glm::vec3 val)
    {
        _position_widget->set_value(val.x, val.y, val.z);
    }

    void transform_widget::set_rotation(float x, float y, float z)
    {
        _rotation_widget->set_value(x, y, z);
    }

    void transform_widget::set_rotation(glm::vec3 val)
    {
        _rotation_widget->set_value(val.x, val.y, val.z);
    }

    void transform_widget::set_scale(float x, float y, float z)
    {
        if (_scale_widget != nullptr)
        {
            _scale_widget->set_value(x, y, z);
        }
    }

    void transform_widget::set_scale(glm::vec3 val)
    {
        if (_scale_widget != nullptr)
        {
            _scale_widget->set_value(val.x, val.y, val.z);
        }
    }

    void transform_widget::set_position_step(float step)
    {
        _position_widget->set_step(step);
    }

    void transform_widget::set_rotation_step(float step)
    {
        _rotation_widget->set_step(step);
    }

    void transform_widget::set_scale_step(float step)
    {
        _scale_widget->set_step(step);
    }
} // namespace cathedral::editor