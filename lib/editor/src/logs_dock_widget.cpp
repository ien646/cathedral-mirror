#include <cathedral/editor/logs_dock_widget.hpp>

#include <QTimer>

namespace cathedral::editor
{
    logs_dock_widget::logs_dock_widget(QWidget* parent)
        : QDockWidget(parent)
    {
        _timer = new QTimer(this);

        connect(_timer, &QTimer::timeout, this, [this] {

        });
        NOT_IMPLEMENTED();
    }
} // namespace cathedral::editor