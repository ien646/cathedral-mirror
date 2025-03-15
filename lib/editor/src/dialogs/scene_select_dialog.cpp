#include <cathedral/editor/dialogs/scene_select_dialog.hpp>

#include <cathedral/editor/styling.hpp>
#include <cathedral/editor/utils.hpp>

#include <cathedral/project/project.hpp>

#include <QApplication>
#include <QLabel>
#include <QListWidget>
#include <QPushButton>
#include <QSplitter>
#include <QStyleFactory>
#include <QVBoxLayout>

namespace cathedral::editor
{
    scene_select_dialog::scene_select_dialog(project::project& pro, QWidget* parent)
        : QDialog(parent)
        , _project(pro)
    {
        auto* main_layout = new QVBoxLayout;
        setLayout(main_layout);

        auto* content_splitter = new QSplitter(Qt::Orientation::Horizontal);
        main_layout->addWidget(content_splitter);

        auto* list_widget = new QListWidget;
        content_splitter->addWidget(list_widget);

        auto* preview_widget = new QLabel;
        content_splitter->addWidget(preview_widget);

        auto* buttons_layout = new QHBoxLayout;
        main_layout->addLayout(buttons_layout);

        auto* cancel_button = new QPushButton("Cancel");
        auto* select_button = new QPushButton("Select");
        select_button->setDisabled(true);
        buttons_layout->addStretch(1);
        buttons_layout->addWidget(cancel_button, 0);
        buttons_layout->addWidget(select_button, 0);

        for (const auto& scene : _project.available_scenes())
        {
            list_widget->addItem(QSTR(scene));
        }

        connect(cancel_button, &QPushButton::clicked, this, [this] {
            _selected_scene = {};
            reject();
        });

        connect(select_button, &QPushButton::clicked, this, [this] { accept(); });

        connect(list_widget, &QListWidget::itemSelectionChanged, this, [this, list_widget, select_button] {
            if (list_widget->selectedItems().empty())
            {
                _selected_scene = {};
                select_button->setDisabled(true);
            }
            else
            {
                _selected_scene = list_widget->selectedItems()[0]->text().toStdString();
                select_button->setDisabled(false);
            }
        });
    }

    const std::string& scene_select_dialog::selected_scene() const
    {
        return _selected_scene;
    }
} // namespace cathedral::editor