#include <cathedral/editor/asset_managers/font_manager.hpp>

#include <QHBoxLayout>
#include <QLabel>
#include <QListWidget>
#include <QPushButton>

namespace cathedral::editor
{
    font_manager::font_manager(project::project& pro, QWidget* parent)
        : QMainWindow(parent)
        , _project(pro)
    {
        auto* main_widget = new QWidget;
        setCentralWidget(main_widget);

        auto* main_layout = new QHBoxLayout;
        main_widget->setLayout(main_layout);

        auto* list_layout = new QVBoxLayout;

        _list_widget = new QListWidget;
        list_layout->addWidget(_list_widget);

        auto* buttons_layout = new QHBoxLayout;
        auto* import_button = new QPushButton("Import");
        auto* rename_button = new QPushButton("Rename");
        auto* delete_button = new QPushButton("Delete");
        buttons_layout->addWidget(import_button);
        buttons_layout->addWidget(rename_button);
        buttons_layout->addWidget(delete_button);

        list_layout->addLayout(buttons_layout);

        main_layout->addLayout(list_layout);

        auto* image_label = new QLabel("image");
        main_layout->addWidget(image_label);


    }
} // namespace cathedral::editor