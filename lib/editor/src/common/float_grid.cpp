#include <cathedral/editor/common/float_grid.hpp>

#include <cathedral/editor/common/float_edit.hpp>

#include <QGridLayout>
#include <cstddef>

namespace cathedral::editor
{
    float_grid::float_grid(glm::uvec2 dims, QWidget* parent)
        : QWidget(parent)
        , _columns(dims.x)
        , _rows(dims.y)
    {
        _values.resize(static_cast<size_t>(_columns) * _rows, 0.0F);

        auto* layout = new QGridLayout;
        setLayout(layout);

        for (unsigned int col = 0; col < _columns; ++col)
        {
            for (unsigned int row = 0; row < _rows; ++row)
            {
                auto* widget = new float_edit;
                layout->addWidget(widget);
                connect(widget, &float_edit::editingFinished, this, [this, widget, col, row] {
                    bool ok = false;
                    const float value = widget->text().toFloat(&ok);
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