#include <cathedral/editor/common/slider.hpp>

#include <QMouseEvent>
#include <QPainter>
#include <QTextItem>

namespace cathedral::editor
{
    slider::slider(QWidget* parent, QString text)
        : QWidget(parent)
        , _text(std::move(text))
    {
        setSizePolicy(QSizePolicy::Policy::Fixed, QSizePolicy::Policy::Fixed);
        setMouseTracking(true);
        setFixedSize(static_cast<int>(8.0 * devicePixelRatio()), static_cast<int>(8.0 * devicePixelRatio()));
        setCursor(QCursor(Qt::CursorShape::SizeHorCursor));
    }

    void slider::set_background_color(QColor color)
    {
        _background_color = color;
    }

    void slider::set_step(float step)
    {
        _step_per_pixel = step;
    }

    void slider::set_text(QString text)
    {
        _text = std::move(text);
    }

    void slider::paintEvent([[maybe_unused]] QPaintEvent* ev)
    {
        QPainter painter(this);
        painter.setRenderHint(QPainter::RenderHint::Antialiasing, true);
        painter.setRenderHint(QPainter::RenderHint::VerticalSubpixelPositioning, true);
        painter.setPen(QPen(QBrush(0xFFFFFF), 1));
        painter.setBrush(QBrush(_background_color));

        QRect drect = rect();
        drect.setWidth(rect().width());
        drect.setX(2);
        drect.setHeight(rect().height());
        drect.setY(2);
        painter.fillRect(drect, QBrush(_background_color));

        painter.drawText(drect, _text, QTextOption(Qt::AlignmentFlag::AlignCenter));
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