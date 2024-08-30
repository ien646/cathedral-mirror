#include <cathedral/editor/styling.hpp>

#include <QStyleFactory>

namespace cathedral::editor
{
    QPalette get_editor_palette()
    {
        QPalette palette;
        palette.setBrush(QPalette::ColorRole::Accent, QColor(0x888888));
        palette.setBrush(QPalette::ColorRole::AlternateBase, QColor(0xC5C5C5));
        palette.setBrush(QPalette::ColorRole::Base, QColor(0xB5B5B5));
        palette.setBrush(QPalette::ColorRole::BrightText, QColor(0x101020));
        palette.setBrush(QPalette::ColorRole::Button, QColor(0xC2C2C2));
        palette.setBrush(QPalette::ColorRole::ButtonText, Qt::black);
        palette.setBrush(QPalette::ColorRole::Mid, QColor(0xAAAAAA));
        palette.setBrush(QPalette::ColorRole::Midlight, QColor(0xBBBBBB));
        palette.setBrush(QPalette::ColorRole::Highlight, QBrush(QColor(0x808090), Qt::BrushStyle::Dense4Pattern));
        palette.setBrush(QPalette::ColorRole::HighlightedText, QColor(0x000020));
        palette.setBrush(QPalette::ColorRole::Text, Qt::black);
        palette.setBrush(QPalette::ColorRole::Window, QColor(0xB5B5B5));
        palette.setBrush(QPalette::ColorRole::WindowText, Qt::black);

        palette.setBrush(QPalette::ColorRole::Dark, QColor(0x333333));
        palette.setBrush(QPalette::ColorRole::Light, QColor(0x444444));
        palette.setBrush(QPalette::ColorRole::Shadow, QColor(0x808080));

        palette.setBrush(QPalette::ColorGroup::Disabled, QPalette::ButtonText, QColor(0xAAAAAA));
        return palette;
    }

    QStyle* get_editor_style()
    {
        return QStyleFactory::create("windows");
    }

    QFont get_editor_font()
    {
        return { "monospace", 8 };
    }
} // namespace cathedral::editor