#pragma once

#include <cathedral/core.hpp>

#include <QDockWidget>
#include <QFuture>

FORWARD_CLASS_INLINE(QTextEdit);
FORWARD_CLASS_INLINE(QTimer);

namespace cathedral::editor
{
    class logs_dock_widget final : public QDockWidget
    {
    public:
        explicit logs_dock_widget(QWidget* parent = nullptr);

    private:
        QTextEdit* _log;
        QTimer* _timer = nullptr;
        std::future<void> _timer_future;

        void resizeEvent(QResizeEvent* event) override;
    };
}