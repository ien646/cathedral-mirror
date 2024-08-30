#pragma once

#include <QDialog>

namespace cathedral::editor
{
    class text_output_dialog : public QDialog
    {
    public:
        text_output_dialog(QString title, QString label, QString text, QWidget* parent);
    };
} // namespace cathedral::editor