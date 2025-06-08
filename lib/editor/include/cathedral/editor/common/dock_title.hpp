#pragma once

#include <QLabel>

namespace cathedral::editor
{
    class dock_title final : public QLabel
    {
    public:
        explicit dock_title(const QString& text, QWidget* parent = nullptr);
    };
} // namespace cathedral::editor