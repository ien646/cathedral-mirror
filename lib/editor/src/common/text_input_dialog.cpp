#include <cathedral/editor/common/text_input_dialog.hpp>

#include <cathedral/editor/common/message.hpp>

#include <QHBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QMessageBox>
#include <QPushButton>

namespace cathedral::editor
{
    text_input_dialog::
        text_input_dialog(QWidget* parent, QString title, QString label, bool allow_empty, QString placeholder)
    {
        setModal(true);

        setWindowTitle(title);

        auto* layout = new QHBoxLayout(this);
        setLayout(layout);

        layout->addWidget(new QLabel(label, this), 0);

        auto* line_edit = new QLineEdit(this);
        line_edit->setText(placeholder);

        layout->addWidget(line_edit, 1);

        auto* accept_button = new QPushButton(this);
        accept_button->setText("Accept");

        layout->addWidget(accept_button, 0);

        adjustSize();

        connect(accept_button, &QPushButton::clicked, this, [this, line_edit, allow_empty] {
            if (line_edit->text().isEmpty())
            {
                if (allow_empty)
                {
                    _result = {};
                    accept();
                }
                else
                {
                    show_error_message("Invalid input. Empty text is not allowed", this);
                }
            }
            else
            {
                _result = line_edit->text();
                accept();
            }
        });
    }
} // namespace cathedral::editor