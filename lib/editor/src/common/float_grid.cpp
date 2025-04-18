#include <cathedral/editor/common/float_grid.hpp>

#include <cathedral/editor/common/float_edit.hpp>

#include <QGridLayout>
#include <cstddef>

namespace cathedral::editor
{
    float_grid::float_grid(glm::uvec2 dims, QWidget* parent)
        : QWidget(parent)
        , _columns(dims.y)
        , _rows(dims.x)
    {
        _values.resize(static_cast<size_t>(_columns) * _rows, 0.0F);

        auto* layout = new QGridLayout;
        setLayout(layout);

        for (unsigned int col = 0; col < _columns; ++col)
        {
            for (unsigned int row = 0; row < _rows; ++row)
            {
                auto* widget = new float_edit;
                layout->addWidget(widget, static_cast<int>(row), static_cast<int>(col));
                connect(widget, &float_edit::editingFinished, this, [this, widget, col, row] {
                    bool ok = false;
                    const float value = widget->text().toFloat(&ok);
                    if (ok)
                    {
                        _values[(col * _rows) + row] = value;
                        emit value_changed(_values);
                    }
                });
                _widgets.push_back(widget);
            }
        }
    }

    bool float_grid::set_value(unsigned int row, unsigned int col, float value)
    {
        if (row >= _rows || col >= _columns)
        {
            return false;
        }

        _values[(row * _columns) + col] = value;
        update_grid();
        return true;
    }

    void float_grid::update_grid()
    {
        for (size_t i = 0; i < _values.size(); ++i)
        {
            _widgets[i]->set_value(_values[i]);
        }
    }
} // namespace cathedral::editor