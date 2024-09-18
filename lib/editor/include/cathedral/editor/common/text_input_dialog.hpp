#pragma once

#include <QDialog>

namespace cathedral::editor
{
    class text_input_dialog : public QDialog
    {
        Q_OBJECT

    public:
        text_input_dialog(
            QWidget* parent,
            const QString& title,
            const QString& label,
            bool allow_empty,
            const QString& placeholder = "");

        QString result() const { return _result; }

    private:
        QString _result;
    };
} // namespace cathedral::editor