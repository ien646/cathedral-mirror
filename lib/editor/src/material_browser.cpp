#include <cathedral/editor/material_browser.hpp>

#include <cathedral/project/project.hpp>

#include <QFormLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QListWidget>

namespace cathedral::editor
{
    material_browser::material_browser(project::project& project, QWidget* parent)
        : QWidget(parent)
        , _project(project)
    {
        auto* main_layout = new QHBoxLayout(this);
        setLayout(main_layout);

        auto* mat_list = new QListWidget;
        mat_list->setSelectionMode(QAbstractItemView::SelectionMode::SingleSelection);
        for (const auto& name : _project.material_assets() | std::views::keys)
        {
            mat_list->addItem(QString::fromStdString(name));
        }
        main_layout->addWidget(mat_list);

        auto* properties_layout = new QFormLayout(this);
        main_layout->addLayout(properties_layout);

        connect(mat_list, &QListWidget::itemSelectionChanged, this, [=, this]() mutable {
            delete properties_layout;
            properties_layout = new QFormLayout;

            if (mat_list->selectedItems().empty())
            {
                return;
            }

            const auto name = mat_list->selectedItems().at(0)->text().toStdString();
            const auto& asset = _project.material_assets().at(name);

            const auto qlabel_fromstr = [](const auto& str) { return new QLabel(QString::fromStdString(str)); };

            properties_layout->addRow("Vertex shader: ", qlabel_fromstr(asset->vertex_shader_ref()));
            properties_layout->addRow("Fragment shader: ", qlabel_fromstr(asset->fragment_shader_ref()));
        });
    }
} // namespace cathedral::editor