#pragma once

#include <QWidget>

namespace cathedral::editor
{
    class float_grid : public QWidget
    {
    public:
        float_grid(unsigned int cols, unsigned int rows, QWidget* parent);

    signals:
        void value_changed(const std::vector<float>& values);

    private:
        unsigned int _columns;
        unsigned int _rows;
        std::vector<float> _values;
    };
} // namespace cathedral::editor