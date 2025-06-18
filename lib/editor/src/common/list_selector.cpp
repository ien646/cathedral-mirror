#include <QLabel>
#include <cathedral/editor/common/list_selector.hpp>

#include <QListWidget>
#include <QVBoxLayout>

namespace cathedral::editor
{
    list_selector::list_selector(QWidget* parent, std::vector<list_selector_item> list)
        : QDialog(parent)
        , _list(std::move(list))
    {
        auto* layout = new QVBoxLayout(this);
        setLayout(layout);

        auto* list_widget = new QListWidget(this);
        list_widget->setSelectionBehavior(QAbstractItemView::SelectionBehavior::SelectRows);
        list_widget->setSelectionMode(QAbstractItemView::SelectionMode::SingleSelection);

        for (const auto& [text, icon] : _list)
        {
            auto* list_item = new QListWidgetItem(list_widget);
            list_item->setText(text);

            auto* item_widget = new QWidget(this);
            auto* item_layout = new QHBoxLayout(item_widget);

            auto* item_icon = new QLabel(this);
            item_icon->setPixmap(icon);

            auto* item_text = new QLabel(text, this);

            item_layout->addWidget(item_icon, 0);
            item_layout->addWidget(item_text, 1);

            list_widget->addItem(list_item);
            list_widget->setItemWidget(list_item, item_widget);
        }

        connect(list_widget, &QListWidget::itemSelectionChanged, this, [this, list_widget] {
            if (list_widget->selectedItems().size() > 0)
            {
                _result = list_widget->selectedItems()[0]->text();
            }
            else
            {
                _result = {};
            }
        });
    }
} // namespace cathedral::editor