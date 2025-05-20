#include <cathedral/editor/common/slider.hpp>

#include <cathedral/editor/utils.hpp>

#include <QFontMetrics>
#include <QMouseEvent>
#include <QPainter>
#include <QTextItem>

namespace cathedral::editor
{
    slider::slider(QWidget* parent, const QString& text)
        : QLabel(parent)
    {
        setSizePolicy(QSizePolicy::Policy::Fixed, QSizePolicy::Policy::Fixed);
        setMouseTracking(true);
        setCursor(QCursor(Qt::CursorShape::SizeHorCursor));

        setText(text);
    }

    void slider::set_background_color(QColor color)
    {
        _background_color = color;
        setStyleSheet(QSTR(
            "QLabel{{ background-color: rgb({}, {}, {}); color: white; padding: 2px; border-style: solid; "
            "border-color: black; border-width: 1px; }}"
            "QLabel:hover{{ border-color: yellow; }}",
            color.red(),
            color.green(),
            color.blue()));
    }

    void slider::set_step(float step)
    {
        _step_per_pixel = step;
    }

    void slider::set_text(const QString& text)
    {
        setText(text);
    }

    void slider::mousePressEvent(QMouseEvent* ev)
    {
        init_pointer_locker();
        if (ev->button() == Qt::MouseButton::LeftButton)
        {
            _press_pivot = ev->pos().x();
            _holding = true;
            _pointer_locker->lock_pointer();
        }
    }

    void slider::mouseMoveEvent(QMouseEvent* ev)
    {
        if (_holding && ev->button() == Qt::MouseButton::LeftButton)
        {
            const auto current_pos = ev->pos().x();
            const auto diff = static_cast<float>(current_pos - _press_pivot);
            emit value_moved(diff * _step_per_pixel);

            _press_pivot = current_pos;
            _pointer_locker->lock_pointer();
        }
    }

    void slider::mouseReleaseEvent(QMouseEvent* ev)
    {
        if (_holding && ev->button() == Qt::MouseButton::LeftButton)
        {
            _holding = false;
            _pointer_locker->unlock_pointer();
        }
    }

    void slider::mouseMoveWhileLocked(const QPoint delta)
    {
        emit value_moved(static_cast<float>(delta.x()) * _step_per_pixel);
    }

    void slider::init_pointer_locker()
    {
        if (!_pointer_locker)
        {
            _pointer_locker = std::make_unique<pointer_locker>(this);

            connect(_pointer_locker.get(), &pointer_locker::mouseMovementDelta, this, [this](const QPoint delta) {
                mouseMoveWhileLocked(delta);
            });
        }
    }
} // namespace cathedral::editor