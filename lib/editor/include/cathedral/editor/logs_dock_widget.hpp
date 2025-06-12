#pragma once

#include <cathedral/core.hpp>

#include <QDockWidget>

FORWARD_CLASS_INLINE(QTimer);

namespace cathedral::editor
{
    class logs_dock_widget final : public QDockWidget
    {
    public:
        explicit logs_dock_widget(QWidget* parent = nullptr);

    private:
        QTimer* _timer = nullptr;
    };
}