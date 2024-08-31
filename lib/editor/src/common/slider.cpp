#include <cathedral/editor/common/slider.hpp>

#include <QMouseEvent>
#include <QPainter>
#include <QTextItem>

namespace cathedral::editor
{
    slider::slider(QWidget* parent)
        : QWidget(parent)
    {
        setSizePolicy(QSizePolicy::Policy::Fixed, QSizePolicy::Policy::Fixed);
        setMouseTracking(true);
        setFixedSize(8 * devicePixelRatio(), 8 * devicePixelRatio());
        setCursor(QCursor(Qt::CursorShape::SizeHorCursor));
    }

    void slider::set_step(float step)
    {
        _step_per_pixel = step;
    }

    void slider::paintEvent(QPaintEvent* ev)
    {
        QPainter painter(this);
        painter.setRenderHint(QPainter::RenderHint::Antialiasing, true);
        painter.setRenderHint(QPainter::RenderHint::VerticalSubpixelPositioning, true);
        painter.setPen(QPen(QBrush(0xFFFFFF), 1));
        painter.setBrush(QBrush(0x888888, Qt::BrushStyle::Dense5Pattern));

        QRect drect = rect();
        drect.setWidth(rect().width() - 4);
        drect.setX(2);
        drect.setHeight(rect().height() - 4);
        drect.setY(2);
        painter.drawRect(drect);
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
            const float diff = current_pos - _press_pivot;
            emit value_moved(diff * _step_per_pixel);

            _press_pivot = current_pos;
        }
    }

    void slider::mouseReleaseEvent(QMouseEvent* ev)
    {
        _holding = false;
    }
} // namespace cathedral::editor