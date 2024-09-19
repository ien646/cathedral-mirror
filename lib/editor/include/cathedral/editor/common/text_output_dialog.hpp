#pragma once

#include <QDialog>

namespace cathedral::editor
{
    class text_output_dialog : public QDialog
    {
    public:
        text_output_dialog(const QString& title, const QString& label, const QString& text, QWidget* parent);
    };
} // namespace cathedral::editor