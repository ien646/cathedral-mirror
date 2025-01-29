#include <cathedral/editor/asset_managers/mesh_manager.hpp>

#include <cathedral/editor/asset_managers/dialogs/new_mesh_dialog.hpp>
#include <cathedral/editor/common/mesh_viewer.hpp>

#include <cathedral/project/assets/mesh_asset.hpp>

#include "ui_mesh_manager.h"

namespace cathedral::editor
{
    mesh_manager::mesh_manager(project::project& pro, QWidget* parent)
        : QMainWindow(parent)
        , resource_manager_base(pro)
        , _ui(new Ui::mesh_manager)
    {
        _ui->setupUi(this);

        connect(_ui->actionClose, &QAction::triggered, this, &mesh_manager::close);
        connect(_ui->item_manager, &item_manager::add_clicked, this, &mesh_manager::slot_add_mesh_clicked);
        connect(_ui->item_manager, &item_manager::rename_clicked, this, &mesh_manager::slot_rename_mesh_clicked);
        connect(_ui->item_manager, &item_manager::delete_clicked, this, &mesh_manager::slot_delete_mesh_clicked);
        connect(_ui->item_manager, &item_manager::item_selection_changed, this, &mesh_manager::slot_mesh_selection_changed);
    }

    item_manager* mesh_manager::get_item_manager_widget()
    {
        return _ui->item_manager;
    }

    const item_manager* mesh_manager::get_item_manager_widget() const
    {
        return _ui->item_manager;
    }

    void mesh_manager::showEvent([[maybe_unused]] QShowEvent* event)
    {
        reload_item_list();
    }

    void mesh_manager::slot_add_mesh_clicked()
    {
        auto* diag = new new_mesh_dialog(_ui->item_manager->get_texts(), this);
        if (diag->exec() == QDialog::DialogCode::Accepted)
        {
            const auto& name = diag->name();
            const auto& path = diag->path();

            auto new_asset = std::make_shared<project::mesh_asset>(
                _project,
                _project.name_to_abspath<project::mesh_asset>(name.toStdString()));

            engine::mesh mesh(path.toStdString());
            new_asset->save_mesh(mesh);
            new_asset->mark_as_manually_loaded();
            new_asset->save();

            _project.add_asset(std::move(new_asset));
        }
        reload_item_list();
    }

    void mesh_manager::slot_rename_mesh_clicked()
    {
        rename_asset();
    }

    void mesh_manager::slot_delete_mesh_clicked()
    {
        delete_asset();
    }

    void mesh_manager::slot_mesh_selection_changed(std::optional<QString> selected)
    {
        if (!selected.has_value() || selected.value().isEmpty())
        {
            return;
        }

        const std::string& selected_value = selected->toStdString();
        const std::string path = _project.name_to_abspath<project::mesh_asset>(selected_value);
        const auto& mesh = std::make_shared<engine::mesh>(get_assets().at(path)->load_mesh());

        _ui->mesh_viewer->set_mesh(mesh);
    }
} // namespace cathedral::editor
