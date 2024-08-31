#pragma once

#include <QWidget>

#include <glm/vec3.hpp>

class QHBoxLayout;

namespace cathedral::editor
{
    class sliding_float;

    class vector3_widget : public QWidget
    {
        Q_OBJECT

    public:
        vector3_widget(QWidget* parent = nullptr);

        glm::vec3 get_value() const;
        void set_value(float x, float y, float z);

        void set_step(float step);

    private:
        QHBoxLayout* _main_layout = nullptr;
        sliding_float* _slider_x = nullptr;
        sliding_float* _slider_y = nullptr;
        sliding_float* _slider_z = nullptr;

    signals:
        void value_changed(float x, float y, float z);
    };
} // namespace cathedral::editor