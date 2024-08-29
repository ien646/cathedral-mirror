#pragma once

#include <QFont>
#include <QPalette>
#include <QStyle>

namespace cathedral::editor
{
    QPalette get_editor_palette();
    QStyle* get_editor_style();
    QFont get_editor_font();
    QString get_editor_stylesheet();
} // namespace cathedral::editor