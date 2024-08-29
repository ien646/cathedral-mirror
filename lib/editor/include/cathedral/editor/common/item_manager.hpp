#pragma once

#include <QListWidget>
#include <QListWidgetItem>
#include <QPushButton>
#include <QWidget>

namespace cathedral::editor
{
    class item_manager : public QWidget
    {
        Q_OBJECT
        
    public:
        item_manager(QWidget* parent);

        void add_item(QString item);

        [[nodiscard]] bool select_item(QString text);
        std::optional<QString> current_text();
        std::optional<QListWidgetItem*> current_item();
        void clear_selection();
        void clear_items();

        void sort_items(Qt::SortOrder order);

        QStringList get_texts();

    signals:
        void add_clicked();
        void rename_clicked(std::optional<QString> selected);
        void delete_clicked(std::optional<QString> selected);
        void item_selection_changed(std::optional<QString> selected);

    private:
        QListWidget* _list = nullptr;
        QPushButton* _addButton = nullptr;
        QPushButton* _renameButton = nullptr;
        QPushButton* _deleteButton = nullptr;

    private slots:
        void slot_rename_clicked();
        void slot_delete_clicked();
        void slot_item_selection_changed();
    };
}