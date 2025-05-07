#include <cathedral/editor/styling.hpp>

#include <QFontDatabase>
#include <QStyle>
#include <QStyleFactory>

namespace cathedral::editor
{
    const auto background = QColor(0xBBAA88);
    const auto background_highlight = QColor(0xA8C8A8);
    const auto content_light = QColor(0xF0D08E);
    const auto content_medium = QColor(0xE2A97E);
    const auto content_dark = QColor(0xCB8075);
    const auto foreground = QColor(0x655057);
    const auto foreground_dark = QColor(0x161314);
    const auto foreground_highlight = QColor(0x6D8D8A);

    const QPalette& get_editor_palette()
    {
        static const auto spalette = [] {
            QPalette palette;
            palette.setBrush(QPalette::ColorRole::Accent, foreground_highlight);
            palette.setBrush(QPalette::ColorRole::AlternateBase, background_highlight);
            palette.setBrush(QPalette::ColorRole::Base, background);
            palette.setBrush(QPalette::ColorRole::BrightText, foreground_dark);
            palette.setBrush(QPalette::ColorRole::Button, content_medium);
            palette.setBrush(QPalette::ColorRole::ButtonText, foreground_dark);
            palette.setBrush(QPalette::ColorRole::Dark, content_dark);
            palette.setBrush(QPalette::ColorRole::Highlight, QBrush(background_highlight, Qt::BrushStyle::Dense4Pattern));
            palette.setBrush(QPalette::ColorRole::HighlightedText, foreground);
            palette.setBrush(QPalette::ColorRole::Light, content_light);
            palette.setBrush(QPalette::ColorRole::Mid, content_medium);
            palette.setBrush(QPalette::ColorRole::Midlight, content_medium);
            palette.setBrush(QPalette::ColorRole::PlaceholderText, foreground_highlight);
            palette.setBrush(QPalette::ColorRole::Shadow, content_dark);
            palette.setBrush(QPalette::ColorRole::Text, foreground_dark);
            palette.setBrush(QPalette::ColorRole::Window, background);
            palette.setBrush(QPalette::ColorRole::WindowText, foreground_dark);

            palette.setBrush(QPalette::ColorRole::Shadow, background_highlight);

            palette.setBrush(QPalette::ColorGroup::Disabled, QPalette::ButtonText, QColor(0x806570));
            return palette;
        }();

        return spalette;
    }

    QStyle* get_editor_style()
    {
        return QStyleFactory::create("fusion");
    }

    QFont get_editor_font()
    {
        static const auto font = []() -> QFont {
            QFontDatabase::addApplicationFont(":/fonts/mono");
            return { "JetBrainsMono", 8 };
        }();
        return font;
    }

    QString get_editor_stylesheet()
    {
        return R"css(
            QSpinBox::down-button, QSpinBox::up-button {
                font-weight: bold;
            }

            QSplitter::handle {
                background-color: rgba(128, 128, 128, 50);
            }
        )css";
    }
} // namespace cathedral::editor