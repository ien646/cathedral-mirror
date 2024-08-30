#pragma once

#include <QHBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QWidget>

#include <cathedral/editor/common/slider.hpp>

namespace cathedral::editor
{
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
        QLineEdit* _line_edit = nullptr;
        slider* _slider = nullptr;
        float _current_value = 0.0f;

        // Limit amount of updates per second
        QTimer* _update_timer = nullptr;
        bool _update_semaphore = true;

    signals:
        void value_changed(float value);
    };
} // namespace cathedral::editor