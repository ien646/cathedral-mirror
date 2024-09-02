#include <cathedral/editor/common/item_manager.hpp>

#include <QHBoxLayout>
#include <QLineEdit>
#include <QListWidget>
#include <QListWidgetItem>
#include <QPushButton>
#include <QVBoxLayout>

namespace cathedral::editor
{
    item_manager::item_manager(QWidget* parent)
        : QWidget(parent)
    {
        _list = new QListWidget;
        _list->setSelectionBehavior(QAbstractItemView::SelectionBehavior::SelectItems);
        _list->setSelectionMode(QAbstractItemView::SelectionMode::SingleSelection);

        _addButton = new QPushButton("Add");
        _renameButton = new QPushButton("Rename");
        _deleteButton = new QPushButton("Delete");

        _renameButton->setEnabled(false);
        _deleteButton->setEnabled(false);

        _search_edit = new QLineEdit;
        _search_edit->setPlaceholderText("Filter");

        auto* buttonsLayout = new QHBoxLayout;
        buttonsLayout->addWidget(_addButton);
        buttonsLayout->addWidget(_renameButton);
        buttonsLayout->addWidget(_deleteButton);

        auto* mainLayout = new QVBoxLayout;
        mainLayout->addWidget(_search_edit, 0);
        mainLayout->addWidget(_list, 1);
        mainLayout->addLayout(buttonsLayout, 0);
        mainLayout->setContentsMargins(0, 0, 0, 0);

        setLayout(mainLayout);

        connect(_search_edit, &QLineEdit::textChanged, this, &item_manager::slot_search_text_changed);
        connect(_addButton, &QPushButton::clicked, this, &item_manager::add_clicked);
        connect(_renameButton, &QPushButton::clicked, this, &item_manager::slot_rename_clicked);
        connect(_deleteButton, &QPushButton::clicked, this, &item_manager::slot_delete_clicked);
        connect(_list, &QListWidget::itemSelectionChanged, this, &item_manager::slot_item_selection_changed);
    }

    void item_manager::add_item(QString item)
    {
        _list->addItem(item);
    }

    bool item_manager::select_item(QString text)
    {
        const auto items = _list->findItems(text, Qt::MatchFlag::MatchExactly);
        if (items.empty())
        {
            return false;
        }

        _list->setCurrentItem(items[0]);
        return true;
    }

    QString item_manager::current_text() const
    {
        if (_list->selectedItems().empty())
        {
            return {};
        }
        return _list->selectedItems()[0]->text();
    }

    QListWidgetItem* item_manager::current_item()
    {
        if (_list->selectedItems().empty())
        {
            return nullptr;
        }
        return _list->selectedItems()[0];
    }

    void item_manager::clear_selection()
    {
        _list->clearSelection();
    }

    void item_manager::clear_items()
    {
        _list->clear();
    }

    void item_manager::sort_items(Qt::SortOrder order)
    {
        _list->sortItems(order);
    }

    QStringList item_manager::get_texts() const
    {
        QStringList result;
        for (int i = 0; i < _list->count(); ++i)
        {
            result << _list->item(i)->text();
        }
        return result;
    }

    void item_manager::slot_rename_clicked()
    {
        if (_list->selectedItems().empty())
        {
            emit rename_clicked(std::nullopt);
        }
        else
        {
            emit rename_clicked(_list->selectedItems()[0]->text());
        }
    }

    void item_manager::slot_delete_clicked()
    {
        if (_list->selectedItems().empty())
        {
            emit delete_clicked(std::nullopt);
        }
        else
        {
            emit delete_clicked(_list->selectedItems()[0]->text());
        }
    }

    void item_manager::slot_item_selection_changed()
    {
        if (_list->selectedItems().empty())
        {
            _renameButton->setEnabled(false);
            _deleteButton->setEnabled(false);
            emit item_selection_changed(std::nullopt);
        }
        else
        {
            _renameButton->setEnabled(true);
            _deleteButton->setEnabled(true);
            emit item_selection_changed(_list->selectedItems()[0]->text());
        }
    }

    void item_manager::slot_search_text_changed()
    {
        for(int i = 0; i < _list->count(); ++i)
        {
            auto* item = _list->item(i);
            const bool match = item->text().contains(_search_edit->text(), Qt::CaseInsensitive);
            item->setHidden(!match);
        }
    }
} // namespace cathedral::editor