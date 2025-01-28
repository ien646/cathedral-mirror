#include <cathedral/editor/node_properties/mesh_selector.hpp>

#include <cathedral/editor/asset_managers/mesh_manager.hpp>

#include <QApplication>
#include <QHBoxLayout>
#include <QLineEdit>
#include <QPushButton>

namespace cathedral::editor
{
    mesh_selector::mesh_selector(project::project& pro, QWidget* parent, const QString& initial_text)
        : QWidget(parent)
        , _project(pro)
    {
        auto* layout = new QHBoxLayout(this);
        setLayout(layout);

        _line_edit = new QLineEdit(this);
        _line_edit->setText(initial_text);
        layout->addWidget(_line_edit, 1);

        auto* select_button = new QPushButton(this);
        select_button->setText("Select...");
        layout->addWidget(select_button, 0);

        connect(select_button, &QPushButton::clicked, this, [this] { open_select_dialog(); });
    }

    void mesh_selector::set_text(const QString& text)
    {
        _line_edit->setText(text);
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