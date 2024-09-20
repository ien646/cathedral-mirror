#include <cathedral/editor/common/path_selector.hpp>

#include <QFileDialog>
#include <QHBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QStyle>
#include <QStyleOption>

namespace cathedral::editor
{
    path_selector::path_selector(path_selector_mode mode, const QString& label, QWidget* parent)
        : QWidget(parent)
        , _mode(mode)
    {
        _main_layout = new QHBoxLayout;
        _line_edit = new QLineEdit;
        _browse_button = new QPushButton;

        _line_edit->setReadOnly(true);

        _browse_button->setText("Browse");
        _browse_button->setFixedWidth(_browse_button->fontMetrics().horizontalAdvance("Browse") + 8);

        if (!label.isEmpty())
        {
            _label = new QLabel;
            _label->setText(label);
        }

        if (_label != nullptr)
        {
            _main_layout->addWidget(_label, 0, Qt::AlignLeft);
        }
        _main_layout->addWidget(_line_edit, 1);
        _main_layout->addWidget(_browse_button, 0, Qt::AlignRight);
        _main_layout->setContentsMargins(0, 0, 0, 0);

        setLayout(_main_layout);

        connect(_browse_button, &QPushButton::clicked, this, [this] { handle_browse_click(); });
    }

    void path_selector::set_text(const QString& str)
    {
        _line_edit->setText(str);
    }

    QString path_selector::text() const
    {
        return _line_edit->text();
    }

    void path_selector::handle_browse_click()
    {
        const std::string dialog_text = _mode == path_selector_mode::DIRECTORY ? "Select a directory" : "Select a file";
        QFileDialog dialog(this);
        dialog.setFileMode(_mode == path_selector_mode::DIRECTORY ? QFileDialog::FileMode::Directory : QFileDialog::FileMode::ExistingFile);
        dialog.setWindowTitle(QString::fromStdString(dialog_text));
        if (dialog.exec() != 0)
        {
            const auto selected = dialog.selectedFiles();
            _line_edit->setText(selected.at(0));
            if (!selected.empty())
            {
                emit paths_selected(selected);
            }
        }
    }
} // namespace cathedral::editor