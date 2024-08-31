#include <cathedral/editor/common/code_editor.hpp>

#include <QFontMetrics>
#include <QHBoxLayout>
#include <QScrollBar>
#include <QTextBlock>

namespace cathedral::editor
{
    int code_editor_text_widget::first_block_index() const
    {
        return firstVisibleBlock().blockNumber();
    }

    code_editor::code_editor(QWidget* parent)
        : QWidget(parent)
    {
        _layout = new QHBoxLayout(this);
        _layout->setSpacing(0);

        _line_widget = new code_editor_line_widget(this);
        _text_widget = new code_editor_text_widget(this);

        _line_widget->setContentsMargins(0, 0, 0, 0);
        _text_widget->setContentsMargins(0, 0, 0, 0);

        _line_widget->setStyleSheet("QPlainTextEdit{ background-color: #444444; color: #BBBBBB }");

        _text_widget->setLineWrapMode(QPlainTextEdit::LineWrapMode::NoWrap);

        _line_widget->setFixedWidth(QFontMetrics(_line_widget->font()).horizontalAdvance(' ') * 6);
        _line_widget->setHorizontalScrollBarPolicy(Qt::ScrollBarPolicy::ScrollBarAlwaysOff);
        _line_widget->setVerticalScrollBarPolicy(Qt::ScrollBarPolicy::ScrollBarAlwaysOff);

        _layout->addWidget(_line_widget, 0);
        _layout->addWidget(_text_widget, 1);

        setLayout(_layout);

        connect(_text_widget, &QPlainTextEdit::blockCountChanged, this, &code_editor::slot_update_line_widget);
        connect(_text_widget, &QPlainTextEdit::cursorPositionChanged, this, &code_editor::slot_update_line_widget);
        connect(_text_widget, &QPlainTextEdit::updateRequest, this, &code_editor::slot_update_line_widget);
    }

    void code_editor::set_text(QString text)
    {
        _text_widget->setPlainText(text);
    }

    QString code_editor::text() const
    {
        return _text_widget->toPlainText();
    }

    void code_editor::slot_update_line_widget()
    {
        _line_widget->clear();

        const size_t block_count = _text_widget->blockCount();
        const auto start_index = _text_widget->first_block_index();

        if (height() <= 0)
        {
            return;
        }

        QFontMetrics metrics(_text_widget->font());
        const auto total_height = _text_widget->height();
        const auto vlines = std::min<size_t>(block_count, (total_height / metrics.lineSpacing()) - 1);

        for (size_t i = 0; i < vlines; ++i)
        {
            const auto line_index = start_index + i + 1;
            _line_widget->appendPlainText(QString::number(line_index));
        }

        _line_widget->verticalScrollBar()->setValue(0);
    }
} // namespace cathedral::editor