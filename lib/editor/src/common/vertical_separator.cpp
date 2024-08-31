#include <cathedral/editor/common/vertical_separator.hpp>

namespace cathedral::editor
{
    vertical_separator::vertical_separator(QWidget* parent)
        : QFrame(parent)
    {
        setFrameShape(QFrame::HLine);
        setFrameShadow(QFrame::Sunken);
    }
} // namespace cathedral::editor