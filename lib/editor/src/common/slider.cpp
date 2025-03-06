#include <cathedral/editor/common/slider.hpp>

#include <QFontMetrics>
#include <QMouseEvent>
#include <QPainter>
#include <QTextItem>

namespace cathedral::editor
{
    slider::slider(QWidget* parent, QString text)
        : QLabel(parent)
    {
        setSizePolicy(QSizePolicy::Policy::Fixed, QSizePolicy::Policy::Fixed);
        setMouseTracking(true);
        setCursor(QCursor(Qt::CursorShape::SizeHorCursor));

        setText(std::move(text));
    }

    void slider::set_background_color(QColor color)
    {
        _background_color = color;
        setStyleSheet(QString("QLabel{ background-color: rgb(%1, %2, %3); color: white; padding: 2px; border-style: solid; "
                              "border-color: black; border-width: 1px; }")
                          .arg(color.red())
                          .arg(color.green())
                          .arg(color.blue()));
    }

    void slider::set_step(float step)
    {
        _step_per_pixel = step;
    }

    void slider::set_text(QString text)
    {
        setText(std::move(text));
    }

    void slider::mousePressEvent(QMouseEvent* ev)
    {
        if (ev->button() == Qt::MouseButton::LeftButton)
        {
            _press_pivot = ev->pos().x();
            _holding = true;
        }
    }

    void slider::mouseMoveEvent(QMouseEvent* ev)
    {
        if (_holding)
        {
            const auto current_pos = ev->pos().x();
            const auto diff = static_cast<float>(current_pos - _press_pivot);
            emit value_moved(diff * _step_per_pixel);

            _press_pivot = current_pos;
        }
    }

    void slider::mouseReleaseEvent([[maybe_unused]] QMouseEvent* ev)
    {
        _holding = false;
    }
} // namespace cathedral::editor