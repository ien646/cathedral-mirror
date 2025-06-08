#pragma once

#include <QFrame>

namespace cathedral::editor
{
    class vertical_separator final : public QFrame
    {
    public:
        explicit vertical_separator(QWidget* parent);
    };
} // namespace cathedral::editor