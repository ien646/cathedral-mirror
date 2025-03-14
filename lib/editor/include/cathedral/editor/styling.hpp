#pragma once

#include <cathedral/core.hpp>

#include <QFont>
#include <QPalette>

FORWARD_CLASS_INLINE(QStyle);

namespace cathedral::editor
{
    const QPalette& get_editor_palette();
    QStyle* get_editor_style();
    QFont get_editor_font();
    QString get_editor_stylesheet();
} // namespace cathedral::editor