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
        new_material_dialog(QStringList banned_names, const QStringList& material_definitions, QWidget* parent = nullptr);

        const auto& name() const { return _name; }

        const auto& matdef() const { return _matdef; }

    private:
        QStringList _banned_names;
        QString _name;
        QString _matdef;

        QLineEdit* _name_edit = nullptr;
        QComboBox* _matdef_combo = nullptr;

        void handle_create_clicked();
    };
} // namespace cathedral::editor