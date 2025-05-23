#include <cathedral/editor/node_properties/mesh_selector.hpp>

#include <cathedral/editor/asset_managers/mesh_manager.hpp>

#include <QApplication>
#include <QHBoxLayout>
#include <QLabel>
#include <QPushButton>

namespace cathedral::editor
{
    mesh_selector::mesh_selector(project::project* project, QWidget* parent, const QString& initial_text)
        : QWidget(parent)
        , _project(project)
    {
        auto* layout = new QHBoxLayout(this);
        setLayout(layout);

        _text = new QLabel(this);
        _text->setWordWrap(true);
        _text->setText(initial_text);

        layout->addWidget(_text, 1);

        auto* select_button = new QPushButton(this);
        select_button->setText("Select...");
        layout->addWidget(select_button, 0);

        connect(select_button, &QPushButton::clicked, this, [this] { open_select_dialog(); });
    }

    void mesh_selector::set_text(const QString& text) const
    {
        _text->setText(text);
    }

    void mesh_selector::open_select_dialog()
    {
        auto* manager = new mesh_manager(_project, this, true);
        manager->setWindowModality(Qt::WindowModality::ApplicationModal);
        manager->show();

        std::shared_ptr<project::asset> asset;
        connect(manager, &mesh_manager::mesh_selected, this, [&asset](std::shared_ptr<project::mesh_asset> mesh_asset) {
            asset = std::move(mesh_asset);
        });

        while (manager->isVisible())
        {
            QApplication::processEvents();
        }

        emit mesh_selected(std::dynamic_pointer_cast<project::mesh_asset>(asset));
    }
} // namespace cathedral::editor