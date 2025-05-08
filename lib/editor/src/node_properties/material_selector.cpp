#include <cathedral/editor/node_properties/material_selector.hpp>

#include <cathedral/editor/asset_managers/material_manager.hpp>

#include <QApplication>
#include <QHBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <utility>

namespace cathedral::editor
{
    material_selector::material_selector(
        project::project* project,
        std::shared_ptr<engine::scene> scene,
        QWidget* parent,
        const QString& initial_text)
        : QWidget(parent)
        , _project(project)
        , _scene(std::move(scene))
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

    void material_selector::set_text(const QString& text) const
    {
        _text->setText(text);
    }

    void material_selector::open_select_dialog()
    {
        auto* manager = new material_manager(_project, _scene, this, true);
        manager->setWindowModality(Qt::WindowModality::ApplicationModal);
        manager->show();

        std::shared_ptr<project::asset> asset;
        connect(
            manager,
            &material_manager::material_selected,
            this,
            [&asset](std::shared_ptr<project::material_asset> material_asset) { asset = std::move(material_asset); });

        while (manager->isVisible())
        {
            QApplication::processEvents();
        }

        emit material_selected(std::dynamic_pointer_cast<project::material_asset>(asset));
    }
} // namespace cathedral::editor