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

        void set_label(const QString& label) const;
        void set_label_color(QColor color) const;

        float get_value() const;
        void set_value(float val);

        void set_step(float step) const;

        void set_min(float min);
        void set_max(float max);
        void set_range(float min, float max);

    private:
        QHBoxLayout* _main_layout = nullptr;
        float_edit* _float_edit = nullptr;
        slider* _slider = nullptr;
        float _current_value = 0.0F;
        float _min_value = std::numeric_limits<float>::lowest();
        float _max_value = std::numeric_limits<float>::max();

        // Limit amount of updates per second
        QTimer* _update_timer = nullptr;
        bool _update_semaphore = true;

    signals:
        void value_changed(float value);
    };
} // namespace cathedral::editor