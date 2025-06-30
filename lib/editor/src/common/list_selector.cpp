#include <QLabel>
#include <cathedral/editor/common/list_selector.hpp>

#include <QListWidget>
#include <QPushButton>
#include <QVBoxLayout>

namespace cathedral::editor
{
    list_selector::list_selector(QWidget* parent, std::vector<list_selector_item> list)
        : QDialog(parent)
        , _list(std::move(list))
    {
        auto* layout = new QVBoxLayout;
        setLayout(layout);

        auto* list_widget = new QListWidget(this);
        list_widget->setSelectionBehavior(QAbstractItemView::SelectionBehavior::SelectRows);
        list_widget->setSelectionMode(QAbstractItemView::SelectionMode::SingleSelection);

        for (const auto& [text, icon] : _list)
        {
            auto* list_item = new QListWidgetItem(QIcon(icon), text);
            list_widget->addItem(list_item);
        }

        layout->addWidget(list_widget);

        auto* buttons_layout = new QHBoxLayout;
        layout->addLayout(buttons_layout);

        auto* cancel_button = new QPushButton("Cancel", this);
        auto* select_button = new QPushButton("Select", this);
        select_button->setEnabled(false);

        buttons_layout->addWidget(cancel_button);
        buttons_layout->addWidget(select_button);

        connect(list_widget, &QListWidget::itemSelectionChanged, this, [this, list_widget, select_button] {
            if (list_widget->selectedItems().size() > 0)
            {
                _result = list_widget->selectedItems()[0]->text();
                select_button->setEnabled(true);
            }
            else
            {
                _result = {};
                select_button->setEnabled(false);
            }
        });

        connect(cancel_button, &QPushButton::clicked, this, [this] { reject(); });

        connect(select_button, &QPushButton::clicked, this, [this] { accept(); });
    }
} // namespace cathedral::editor