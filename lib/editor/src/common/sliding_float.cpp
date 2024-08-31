#include <cathedral/editor/common/sliding_float.hpp>

#include <QDoubleValidator>
#include <QTimer>

namespace cathedral::editor
{
    sliding_float::sliding_float(QWidget* parent)
        : QWidget(parent)
    {
        setSizePolicy(QSizePolicy::Policy::Expanding, QSizePolicy::Policy::Fixed);
        setMinimumSize(0, 0);
        setContentsMargins(0, 0, 0, 0);

        _main_layout = new QHBoxLayout(this);
        _main_layout->setContentsMargins(0, 0, 0, 0);
        _main_layout->setSpacing(2);

        setLayout(_main_layout);

        _label = new QLabel(this);
        _line_edit = new QLineEdit(this);
        _slider = new slider(this);

        _main_layout->addWidget(_label, 0);
        _main_layout->addWidget(_line_edit, 1);
        _main_layout->setSpacing(0);
        _main_layout->addWidget(_slider, 0, Qt::AlignmentFlag::AlignHCenter);

        auto* validator =
            new QDoubleValidator(std::numeric_limits<double>::lowest(), std::numeric_limits<double>::max(), 4, this);
        validator->setDecimals(4);
        validator->setNotation(QDoubleValidator::Notation::StandardNotation);
        validator->setLocale(QLocale::system());
        _line_edit->setValidator(validator);
        _line_edit->setText("0.00");
        _line_edit->setSizePolicy(QSizePolicy::Policy::Expanding, QSizePolicy::Policy::Fixed);

        _update_timer = new QTimer(this);
        _update_timer->setSingleShot(true);
        _update_timer->setInterval(40); // 25fps

        connect(_slider, &slider::value_moved, this, [this](float inc) {
            const float edit_value = _line_edit->text().toFloat() + inc;
            _current_value = edit_value;
            _line_edit->setText(QString::number(edit_value));
            emit value_changed(edit_value);
        });

        connect(_line_edit, &QLineEdit::textChanged, this, [this] {
            bool ok = false;
            const float value = _line_edit->text().toFloat(&ok);
            if (ok)
            {
                _current_value = value;
            }
        });

        connect(_line_edit, &QLineEdit::editingFinished, this, [this] {
            _current_value = _line_edit->text().toFloat();
            emit value_changed(_current_value);
            _line_edit->clearFocus();
        });

        connect(_update_timer, &QTimer::timeout, this, [this] {
            _update_semaphore = true;
            set_value(_current_value);
        });
    }

    void sliding_float::set_label(const QString& label)
    {
        _label->setText(label);
    }

    float sliding_float::get_value() const
    {
        return _current_value;
    }

    void sliding_float::set_value(float val)
    {
        _current_value = val;
        if (_update_semaphore)
        {
            _update_semaphore = false;
            _update_timer->start();

            const auto text = QString::number(val);
            if (_line_edit->text() != text && !_line_edit->hasFocus())
            {
                _line_edit->setText(QString::number(val));
            }
        }
    }

    void sliding_float::set_step(float step)
    {
        _slider->set_step(step);
    }
} // namespace cathedral::editor