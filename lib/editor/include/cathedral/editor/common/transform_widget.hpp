#pragma once

#include <QWidget>

#include <glm/vec3.hpp>

class QVBoxLayout;

namespace cathedral::editor
{
    class vector3_widget;

    class transform_widget : public QWidget
    {
        Q_OBJECT

    public:
        transform_widget(QWidget* parent, bool disable_scale = false);

        void set_position(float x, float y, float z);
        void set_position(glm::vec3 val);
        void set_rotation(float x, float y, float z);
        void set_rotation(glm::vec3 val);
        void set_scale(float x, float y, float z);
        void set_scale(glm::vec3 val);

        void set_position_step(float step);
        void set_rotation_step(float step);
        void set_scale_step(float step);

    private:
        QVBoxLayout* _main_layout = nullptr;
        vector3_widget* _position_widget = nullptr;
        vector3_widget* _rotation_widget = nullptr;
        vector3_widget* _scale_widget = nullptr;

    signals:
        void position_changed(glm::vec3 position);
        void rotation_changed(glm::vec3 rotation);
        void scale_changed(glm::vec3 scale);
    };
}