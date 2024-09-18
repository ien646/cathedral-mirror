#include <cathedral/editor/common/float_grid.hpp>

#include <cathedral/editor/common/float_edit.hpp>

#include <QGridLayout>

namespace cathedral::editor
{
    float_grid::float_grid(unsigned int cols, unsigned int rows, QWidget* parent)
        : QWidget(parent)
        , _columns(cols)
        , _rows(rows)
    {
        _values.resize(_columns * _rows, 0.0F);

        auto* layout = new QGridLayout;
        setLayout(layout);

        for (size_t col = 0; col < cols; ++col)
        {
            for (size_t row = 0; row < rows; ++row)
            {
                auto* widget = new float_edit;
                layout->addWidget(widget);
                connect(widget, &float_edit::editingFinished, this, [this, widget, col, row] {
                    bool ok = false;
                    float value = widget->text().toFloat(&ok);
                    if (ok)
                    {
                        _values[(col * _rows) + row] = value;
                        emit value_changed(_values);
                    }
                });
            }
        }
    }
} // namespace cathedral::editor