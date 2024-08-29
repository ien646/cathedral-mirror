#include <cathedral/editor/common/vector3_widget.hpp>

#include <cathedral/editor/common/sliding_float.hpp>

#include <QHBoxLayout>
#include <QLabel>

namespace cathedral::editor
{
    vector3_widget::vector3_widget(QWidget* parent)
        : QWidget(parent)
    {
        setSizePolicy(QSizePolicy::Policy::Expanding, QSizePolicy::Policy::Fixed);
        setContentsMargins(0, 0, 0, 0);

        _main_layout = new QHBoxLayout(this);
        setLayout(_main_layout);

        _main_layout->setSpacing(0);
        _main_layout->setContentsMargins(0, 0, 0, 0);

        _slider_x = new sliding_float(this);
        _slider_y = new sliding_float(this);
        _slider_z = new sliding_float(this);

        _main_layout->addWidget(_slider_x, 1, Qt::AlignmentFlag::AlignTop | Qt::AlignmentFlag::AlignCenter);
        _main_layout->addSpacing(4);
        _main_layout->addWidget(_slider_y, 1, Qt::AlignmentFlag::AlignTop | Qt::AlignmentFlag::AlignCenter);
        _main_layout->addSpacing(4);
        _main_layout->addWidget(_slider_z, 1, Qt::AlignmentFlag::AlignTop | Qt::AlignmentFlag::AlignCenter);

        _slider_x->set_label("X");
        _slider_y->set_label("Y");
        _slider_z->set_label("Z");

        connect(_slider_x, &sliding_float::value_changed, this, [this](float x) {
            emit value_changed(x, _slider_y->get_value(), _slider_z->get_value());
        });
        connect(_slider_y, &sliding_float::value_changed, this, [this](float y) {
            emit value_changed(_slider_x->get_value(), y, _slider_z->get_value());
        });
        connect(_slider_z, &sliding_float::value_changed, this, [this](float z) {
            emit value_changed(_slider_x->get_value(), _slider_y->get_value(), z);
        });
    }

    glm::vec3 vector3_widget::get_value() const
    {
        return { _slider_x->get_value(), _slider_y->get_value(), _slider_z->get_value() };
    }

    void vector3_widget::set_value(float x, float y, float z)
    {
        _slider_x->set_value(x);
        _slider_y->set_value(y);
        _slider_z->set_value(z);
    }

    void vector3_widget::set_step(float step)
    {
        _slider_x->set_step(step);
        _slider_y->set_step(step);
        _slider_z->set_step(step);
    }
} // namespace cathedral::editor