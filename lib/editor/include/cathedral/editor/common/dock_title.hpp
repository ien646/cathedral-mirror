#pragma once

#include <QLabel>

namespace cathedral::editor
{
    class dock_title : public QLabel
    {
    public:
        dock_title(const QString& text, QWidget* parent = nullptr);
    };
} // namespace cathedral::editor