#include <cathedral/editor/common/sliding_float.hpp>

#include <cathedral/editor/common/float_edit.hpp>
#include <cathedral/editor/common/slider.hpp>

#include <QDoubleValidator>
#include <QHBoxLayout>
#include <QLabel>
#include <QTimer>

namespace cathedral::editor
{
    sliding_float::sliding_float(QWidget* parent, const QString& label)
        : QWidget(parent)
    {
        setSizePolicy(QSizePolicy::Policy::Expanding, QSizePolicy::Policy::Fixed);
        setMinimumSize(0, 0);
        setContentsMargins(0, 0, 0, 0);

        _main_layout = new QHBoxLayout(this);
        _main_layout->setContentsMargins(0, 0, 0, 0);
        _main_layout->setSpacing(2);

        setLayout(_main_layout);

        _float_edit = new float_edit(this, 4);
        _slider = new slider(this, std::move(label));

        _main_layout->addWidget(_slider, 0, Qt::AlignmentFlag::AlignHCenter);
        _main_layout->addWidget(_float_edit, 1);
        _main_layout->setSpacing(0);

        _float_edit->setText("0.00");
        _float_edit->setSizePolicy(QSizePolicy::Policy::Expanding, QSizePolicy::Policy::Fixed);

        _update_timer = new QTimer(this);
        _update_timer->setSingleShot(true);
        _update_timer->setInterval(40); // 25fps

        connect(_slider, &slider::value_moved, this, [this](float inc) {
            const float edit_value = _float_edit->text().toFloat() + inc;
            _current_value = std::clamp(edit_value, _min_value, _max_value);
            _float_edit->setText(QString::number(_current_value));
            emit value_changed(_current_value);
        });

        connect(_float_edit, &QLineEdit::textChanged, this, [this] {
            bool ok = false;
            const float value = _float_edit->text().toFloat(&ok);
            if (ok)
            {
                _current_value = value;
            }
        });

        connect(_float_edit, &QLineEdit::editingFinished, this, [this] {
            _current_value = std::clamp(_float_edit->text().toFloat(), _min_value, _max_value);
            emit value_changed(_current_value);
            _float_edit->clearFocus();
        });

        connect(_update_timer, &QTimer::timeout, this, [this] {
            _update_semaphore = true;
            set_value(_current_value);
        });

        set_label_color({ 128, 128, 128 });
    }

    void sliding_float::set_label(const QString& label) const
    {
        _slider->set_text(label);
    }

    void sliding_float::set_label_color(const QColor color) const
    {
        _slider->set_background_color(color);
    }

    float sliding_float::get_value() const
    {
        return _current_value;
    }

    void sliding_float::set_value(const float val)
    {
        _current_value = std::clamp(val, _min_value, _max_value);
        if (_update_semaphore)
        {
            _update_semaphore = false;
            _update_timer->start();

            if (const auto text = QString::number(val); _float_edit->text() != text && !_float_edit->hasFocus())
            {
                _float_edit->setText(QString::number(val));
            }
        }
    }

    void sliding_float::set_step(const float step) const
    {
        _slider->set_step(step);
    }

    void sliding_float::set_min(const float min)
    {
        _min_value = min;
    }

    void sliding_float::set_max(const float max)
    {
        _max_value = max;
    }

    void sliding_float::set_range(const float min, const float max)
    {
        set_min(min);
        set_max(max);
    }
} // namespace cathedral::editor