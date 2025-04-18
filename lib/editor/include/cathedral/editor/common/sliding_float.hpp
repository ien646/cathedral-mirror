#pragma once

#include <cathedral/core.hpp>

#include <QWidget>

FORWARD_CLASS_INLINE(QHBoxLayout);
FORWARD_CLASS_INLINE(QLabel);
FORWARD_CLASS_INLINE(QLineEdit);

namespace cathedral::editor
{
    FORWARD_CLASS_INLINE(float_edit);
    FORWARD_CLASS_INLINE(slider);

    class sliding_float : public QWidget
    {
        Q_OBJECT

    public:
        sliding_float(QWidget* parent = nullptr, const QString& label = "");

        void set_label(const QString& label);
        void set_label_color(QColor color);

        float get_value() const;
        void set_value(float val);

        void set_step(float step);

    private:
        QHBoxLayout* _main_layout = nullptr;
        float_edit* _float_edit = nullptr;
        slider* _slider = nullptr;
        float _current_value = 0.0F;

        // Limit amount of updates per second
        QTimer* _update_timer = nullptr;
        bool _update_semaphore = true;

    signals:
        void value_changed(float value);
    };
} // namespace cathedral::editor