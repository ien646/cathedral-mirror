#include <cathedral/editor/common/path_selector.hpp>

#include <QFileDialog>
#include <QHBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>

namespace cathedral::editor
{
    path_selector::path_selector(path_selector_mode mode, const QString& label, QWidget* parent)
        : QWidget(parent)
        , _mode(mode)
    {
        _main_layout = new QHBoxLayout(this);
        _label = new QLabel(this);
        _line_edit = new QLineEdit(this);
        _browse_button = new QPushButton(this);

        _line_edit->setReadOnly(true);

        _label->setText(label);
        _browse_button->setText("...");

        _main_layout->addWidget(_label, 0, Qt::AlignLeft);
        _main_layout->addWidget(_line_edit, 1);
        _main_layout->addWidget(_browse_button, 0, Qt::AlignRight);

        connect(_browse_button, &QPushButton::clicked, this, [this] { handle_browse_click(); });
    }

    void path_selector::set_text(const QString& str)
    {
        _line_edit->setText(str);
    }

    void path_selector::handle_browse_click()
    {
        const std::string dialog_text = _mode == path_selector_mode::DIRECTORY ? "Select a directory" : "Select a file";
        QFileDialog dialog(this);
        dialog.setWindowTitle(QString::fromStdString(dialog_text));
        if (dialog.exec())
        {
            const auto selected = dialog.selectedFiles();
            if (!selected.empty())
            {
                emit paths_selected(selected);
            }
        }
    }
} // namespace cathedral::editor