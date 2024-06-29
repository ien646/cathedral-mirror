#pragma once

#include <QMenuBar>
#include <QWidget>

namespace cathedral::editor
{
    class editor_window_menubar : public QMenuBar
    {
        Q_OBJECT

    public:
        editor_window_menubar(QWidget* parent = nullptr);

    signals:
        // File
        void close_clicked();

        // Help
        void about_clicked();
    };
} // namespace cathedral::editor