#include <cathedral/editor/node_properties/font_selector.hpp>

#include <cathedral/editor/asset_managers/font_manager.hpp>

#include <QApplication>
#include <QHBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <utility>

namespace cathedral::editor
{
    font_selector::font_selector(
        project::project* project,
        engine::scene& scene,
        QWidget* parent,
        const QString& initial_text)
        : QWidget(parent)
        , _project(project)
        , _scene(scene)
    {
        CRITICAL_CHECK_NOTNULL(project);

        setObjectName("font_selector");

        auto* layout = new QHBoxLayout;
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

    void font_selector::set_text(const QString& text) const
    {
        _text->setText(text);
    }

    void font_selector::open_select_dialog()
    {
        auto* manager = new font_manager(*_project, this, true);
        manager->setWindowModality(Qt::WindowModality::ApplicationModal);
        manager->show();

        std::shared_ptr<project::asset> asset;
        connect(
            manager,
            &font_manager::font_selected,
            this,
            [&asset](std::shared_ptr<project::font_asset> font_asset) { asset = std::move(font_asset); });

        while (manager->isVisible())
        {
            QApplication::processEvents();
        }

        emit font_selected(std::dynamic_pointer_cast<project::font_asset>(asset));
    }
} // namespace cathedral::editor