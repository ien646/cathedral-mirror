#pragma once

#include <QWidget>

namespace cathedral::editor
{
    class slider : public QWidget
    {
        Q_OBJECT

    public:
        slider(QWidget* parent = nullptr);

        void set_step(float step);

        void paintEvent(QPaintEvent* ev) override;

        void mousePressEvent(QMouseEvent* ev) override;
        void mouseMoveEvent(QMouseEvent* ev) override;
        void mouseReleaseEvent(QMouseEvent* ev) override;

    private:
        bool _holding = false;
        float _step_per_pixel = 0.01f;
        int _press_pivot = 0;

    signals:
        void value_moved(float inc);
    };
} // namespace cathedral::editor