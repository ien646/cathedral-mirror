#include <cathedral/editor/common/dock_title.hpp>

namespace cathedral::editor
{
    dock_title::dock_title(QString text, QWidget* parent)
        : QLabel(parent)
    {
        setText(text);
        setAutoFillBackground(true);
        setContentsMargins(3, 2, 2, 2);

        QPalette title_palette = palette();
        title_palette.setBrush(QPalette::ColorRole::WindowText, QColor(0x080808));
        title_palette.setBrush(QPalette::ColorRole::Window, QBrush(QColor(0x808080), Qt::BrushStyle::Dense1Pattern));
        setPalette(title_palette);
        
        QFont font = this->font();
        font.setBold(true);
        setFont(font);
    }
} // namespace cathedral::editor