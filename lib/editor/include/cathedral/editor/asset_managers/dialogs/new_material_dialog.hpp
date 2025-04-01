#pragma once

#include <cathedral/core.hpp>

#include <QDialog>

FORWARD_CLASS_INLINE(QComboBox);
FORWARD_CLASS_INLINE(QLineEdit);

namespace cathedral::editor
{
    class new_material_dialog : public QDialog
    {
        Q_OBJECT

    public:
        new_material_dialog(QStringList banned_names, QWidget* parent = nullptr);

        const auto& name() const { return _name; }

    private:
        QStringList _banned_names;
        QString _name;

        QLineEdit* _name_edit = nullptr;

        void handle_create_clicked();
    };
} // namespace cathedral::editor