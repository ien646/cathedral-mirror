#pragma once

#include <QLabel>

namespace cathedral::editor
{
    class slider : public QLabel
    {
        Q_OBJECT

    public:
        explicit slider(QWidget* parent = nullptr, QString text = "");

        void set_background_color(QColor color);

        void set_step(float step);

        void set_text(const QString& text);

        void mousePressEvent(QMouseEvent* ev) override;
        void mouseMoveEvent(QMouseEvent* ev) override;
        void mouseReleaseEvent(QMouseEvent* ev) override;

    private:
        bool _holding = false;
        float _step_per_pixel = 0.01F;
        int _press_pivot = 0;
        QColor _background_color = QColor::fromRgb(0, 0, 0, 0);

    signals:
        void value_moved(float inc);
    };
} // namespace cathedral::editor