#pragma once

#include <cathedral/core.hpp>

#include <QDockWidget>

FORWARD_CLASS_INLINE(QListWidget);
FORWARD_CLASS_INLINE(QTimer);

namespace cathedral::editor
{
    class logs_dock_widget final : public QDockWidget
    {
    public:
        explicit logs_dock_widget(QWidget* parent = nullptr);

    private:
        QListWidget* _list;
        QTimer* _timer = nullptr;
        std::vector<QWidget*> _line_widgets;
        std::unordered_map<std::string, QWidget*> _line_widgets_texts;

        void resizeEvent(QResizeEvent* event) override;
    };
}