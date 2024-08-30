#include <cathedral/editor/common/path_selector.hpp>

namespace cathedral::editor
{
    path_selector::path_selector(QWidget* parent)
        : QWidget(parent)
    {
        _main_layout = new QHBoxLayout(this);
        _line_edit = new QLineEdit(this);
        _browse_button = new QPushButton(this);

        _browse_button->setText("...");

        _main_layout->addWidget(_line_edit);
        _main_layout->addWidget(_browse_button);
    }

    void path_selector::set_text(const QString& str)
    {
        _line_edit->setText(str);
    }
}