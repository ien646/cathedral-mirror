#pragma once

#include <QDialog>

namespace cathedral::editor
{
    class new_shader_dialog : public QDialog
    {
        Q_OBJECT

    public:
        new_shader_dialog(QWidget* parent, bool allow_empty = false);

        const QString& result() const { return _result; }

        const QString& type() const { return _type; }

    private:
        QString _result;
        QString _type;
    };
} // namespace cathedral::editor