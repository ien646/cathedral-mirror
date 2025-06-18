#pragma once

#include <QDialog>
#include <QStringList>

namespace cathedral::editor
{
    struct list_selector_item
    {
        QString text;
        QPixmap icon;
    };

    class list_selector final : public QDialog
    {
    public:
        list_selector(QWidget* parent, std::vector<list_selector_item> list);

        const QString& selected_value() const { return _result; }

    private:
        std::vector<list_selector_item> _list;
        QString _result;
    };
} // namespace cathedral::editor