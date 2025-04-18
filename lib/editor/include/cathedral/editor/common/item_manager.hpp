#pragma once

#include <cathedral/core.hpp>

#include <QWidget>

FORWARD_CLASS_INLINE(QLineEdit);
FORWARD_CLASS_INLINE(QListWidget);
FORWARD_CLASS_INLINE(QListWidgetItem);
FORWARD_CLASS_INLINE(QPushButton);

namespace cathedral::editor
{
    class item_manager : public QWidget
    {
        Q_OBJECT

    public:
        explicit item_manager(QWidget* parent);

        void add_item(const QString& item, const QPixmap& icon = {});

        [[nodiscard]] bool select_item(const QString& text);
        QString current_text() const;
        QListWidgetItem* current_item();
        const QListWidgetItem* current_item() const;
        void clear_selection();
        void clear_items();

        void sort_items(Qt::SortOrder order);

        QStringList get_texts() const;

    signals:
        void add_clicked();
        void rename_clicked(std::optional<QString> selected);
        void delete_clicked(std::optional<QString> selected);
        void item_selection_changed(std::optional<QString> selected);

    private:
        QLineEdit* _search_edit = nullptr;
        QListWidget* _list = nullptr;
        QPushButton* _addButton = nullptr;
        QPushButton* _renameButton = nullptr;
        QPushButton* _deleteButton = nullptr;

    private slots:
        void slot_rename_clicked();
        void slot_delete_clicked();
        void slot_item_selection_changed();
        void slot_search_text_changed();
    };
} // namespace cathedral::editor