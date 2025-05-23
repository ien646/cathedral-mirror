#pragma once

#include <cathedral/core.hpp>

#include <QPlainTextEdit>
#include <QWidget>

FORWARD_CLASS_INLINE(QHBoxLayout);

namespace cathedral::editor
{
    class code_editor_text_widget : public QPlainTextEdit
    {
        Q_OBJECT

    public:
        using QPlainTextEdit::QPlainTextEdit;

        auto get_voffset() const { return contentOffset().y(); }

        int first_block_index() const;
    };

    class code_editor_line_widget : public QPlainTextEdit
    {
        Q_OBJECT

    public:
        using QPlainTextEdit::QPlainTextEdit;
    };

    class code_editor : public QWidget
    {
        Q_OBJECT

    public:
        code_editor(QWidget* parent);

        void set_text(const QString& text);
        QString text() const;

        QPlainTextEdit* text_edit_widget() { return _text_widget; }

    private:
        QHBoxLayout* _layout = nullptr;
        code_editor_line_widget* _line_widget = nullptr;
        code_editor_text_widget* _text_widget = nullptr;

        void handle_update_line_widget();
    };
} // namespace cathedral::editor