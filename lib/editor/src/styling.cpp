#include <cathedral/editor/styling.hpp>

#include <QFontDatabase>
#include <QStyleFactory>

namespace cathedral::editor
{
    const QColor BACKGROUND = QColor(0xBBAA88);
    const QColor BACKGROUND_HIGHLIGHT = QColor(0xA8C8A8);
    const QColor CONTENT_LIGHT = QColor(0xF0D08E);
    const QColor CONTENT_MEDIUM = QColor(0xE2A97E);
    const QColor CONTENT_DARK = QColor(0xCB8075);
    const QColor FOREGROUND = QColor(0x655057);
    const QColor FOREGROUND_DARK = QColor(0x161314);
    const QColor FOREGROUND_HIGHLIGHT = QColor(0x6D8D8A);

    QPalette get_editor_palette()
    {
        QPalette palette;
        palette.setBrush(QPalette::ColorRole::Accent, FOREGROUND_HIGHLIGHT);
        palette.setBrush(QPalette::ColorRole::AlternateBase, BACKGROUND_HIGHLIGHT);
        palette.setBrush(QPalette::ColorRole::Base, BACKGROUND);
        palette.setBrush(QPalette::ColorRole::BrightText, FOREGROUND_DARK);
        palette.setBrush(QPalette::ColorRole::Button, CONTENT_MEDIUM);
        palette.setBrush(QPalette::ColorRole::ButtonText, FOREGROUND_DARK);
        palette.setBrush(QPalette::ColorRole::Dark, CONTENT_DARK);
        palette.setBrush(QPalette::ColorRole::Highlight, QBrush(BACKGROUND_HIGHLIGHT, Qt::BrushStyle::Dense4Pattern));
        palette.setBrush(QPalette::ColorRole::HighlightedText, FOREGROUND);
        palette.setBrush(QPalette::ColorRole::Light, CONTENT_LIGHT);
        palette.setBrush(QPalette::ColorRole::Mid, CONTENT_MEDIUM);
        palette.setBrush(QPalette::ColorRole::Midlight, CONTENT_MEDIUM);
        palette.setBrush(QPalette::ColorRole::PlaceholderText, FOREGROUND_HIGHLIGHT);
        palette.setBrush(QPalette::ColorRole::Shadow, CONTENT_DARK);
        palette.setBrush(QPalette::ColorRole::Text, FOREGROUND_DARK);
        palette.setBrush(QPalette::ColorRole::Window, BACKGROUND);
        palette.setBrush(QPalette::ColorRole::WindowText, FOREGROUND_DARK);

        palette.setBrush(QPalette::ColorRole::Shadow, BACKGROUND_HIGHLIGHT);

        palette.setBrush(QPalette::ColorGroup::Disabled, QPalette::ButtonText, QColor(0x806570));

        return palette;
    }

    QStyle* get_editor_style()
    {
        return QStyleFactory::create("windows");
    }

    QFont get_editor_font()
    {
        static const auto FONT = [] -> QFont {
            QFontDatabase::addApplicationFont(":/fonts/Unispace");
            return { "Unispace", 8 };
        }();
        return FONT;
    }

    QString get_editor_stylesheet()
    {
        return R"css(
            QSpinBox::down-button, QSpinBox::up-button {
                font-weight: bold;
            }
        )css";
    }
} // namespace cathedral::editor