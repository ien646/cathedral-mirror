#pragma once

#include <QWidget>

namespace cathedral::editor
{
    class editor_gamemode final : public QWidget
    {
        Q_OBJECT

    public:
        explicit editor_gamemode(QWidget* parent = nullptr);

    signals:
        void play_clicked();
    };
} // namespace cathedral::editor