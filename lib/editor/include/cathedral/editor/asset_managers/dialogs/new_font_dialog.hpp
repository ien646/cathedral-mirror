#pragma once

#include <QDialog>

namespace cathedral::editor
{
    class new_font_dialog final : public QDialog
    {
    public:
        new_font_dialog(QWidget* parent, const QStringList& forbidden_names);

    private:
        QStringList _forbidden_names;
    };
}