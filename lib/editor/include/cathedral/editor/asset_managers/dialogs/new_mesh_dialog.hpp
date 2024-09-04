#pragma once

#include <cathedral/core.hpp>

#include <QDialog>

FORWARD_CLASS_INLINE(QPushButton);

namespace cathedral::editor
{
    class new_mesh_dialog : public QDialog
    {
    public:
        new_mesh_dialog(QStringList banned_names, QWidget* parent);

        const QString& name() const { return _name; }
        const QString& path() const { return _path; }

    private:
        QStringList _banned_names;

        QString _name{};
        QString _path{};
    };
} // namespace cathedral::editor