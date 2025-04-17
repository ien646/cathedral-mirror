#pragma once

#include <cathedral/core.hpp>

#include <glm/vec2.hpp>

#include <QWidget>

namespace cathedral::editor
{
    FORWARD_CLASS_INLINE(float_edit);

    class float_grid : public QWidget
    {
        Q_OBJECT

    public:
        float_grid(glm::uvec2 dims, QWidget* parent);

        bool set_value(unsigned int row, unsigned int col, float value);

    signals:
        void value_changed(const std::vector<float>& values);

    private:
        unsigned int _columns;
        unsigned int _rows;
        std::vector<float> _values;
        std::vector<float_edit*> _widgets;

        void update_grid();
    };
} // namespace cathedral::editor