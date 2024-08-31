#include <cathedral/editor/common/text_output_dialog.hpp>

#include <QLabel>
#include <QPushButton>
#include <QVBoxLayout>

namespace cathedral::editor
{
    text_output_dialog::text_output_dialog(QString title, QString label, QString text, QWidget* parent)
        : QDialog(parent)
    {
        setWindowTitle(title);

        auto* layout = new QVBoxLayout;
        setLayout(layout);

        auto* label_widget = new QLabel(label);
        layout->addWidget(label_widget, 0);

        auto* text_widget = new QPlainTextEdit;
        text_widget->setPlainText(text);
        text_widget->setReadOnly(true);
        text_widget->setTabStopDistance(QFontMetrics(text_widget->font()).horizontalAdvance(' ') * 4);
        text_widget->setLineWrapMode(QPlainTextEdit::LineWrapMode::NoWrap);
        layout->addWidget(text_widget, 1);

        auto* close_button = new QPushButton;
        close_button->setText("Close");
        layout->addWidget(close_button);

        connect(close_button, &QPushButton::clicked, this, &QDialog::close);

        adjustSize();
    }
} // namespace cathedral::editor