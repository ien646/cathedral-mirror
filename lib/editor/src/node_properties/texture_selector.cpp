#include <cathedral/editor/node_properties/texture_selector.hpp>

#include <cathedral/editor/asset_managers/texture_manager.hpp>

#include <QApplication>
#include <QHBoxLayout>
#include <QLabel>
#include <QPushButton>

namespace cathedral::editor
{
    texture_selector::texture_selector(project::project* project, QWidget* parent, const QString& initial_text)
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

    void texture_selector::set_text(const QString& text) const
    {
        _text->setText(text);
    }

    void texture_selector::open_select_dialog()
    {
        auto* manager = new texture_manager(_project, this, true);
        manager->setWindowModality(Qt::WindowModality::ApplicationModal);
        manager->show();

        std::shared_ptr<project::asset> asset;
        connect(
            manager,
            &texture_manager::texture_selected,
            this,
            [&asset](std::shared_ptr<project::texture_asset> texture_asset) { asset = std::move(texture_asset); });

        while (manager->isVisible())
        {
            QApplication::processEvents();
        }

        emit texture_selected(std::dynamic_pointer_cast<project::texture_asset>(asset));
    }
} // namespace cathedral::editor