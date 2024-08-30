#pragma once

#include <QWidget>

class QHBoxLayout;
class QLabel;
class QLineEdit;

namespace cathedral::editor
{
    class float_edit;
    class slider;

    class sliding_float : public QWidget
    {
        Q_OBJECT

    public:
        sliding_float(QWidget* parent = nullptr);

        void set_label(const QString& label);

        float get_value() const;
        void set_value(float val);

        void set_step(float step);

    private:
        QHBoxLayout* _main_layout = nullptr;
        QLabel* _label = nullptr;
        float_edit* _float_edit = nullptr;
        slider* _slider = nullptr;
        float _current_value = 0.0f;

        // Limit amount of updates per second
        QTimer* _update_timer = nullptr;
        bool _update_semaphore = true;

    signals:
        void value_changed(float value);
    };
} // namespace cathedral::editor