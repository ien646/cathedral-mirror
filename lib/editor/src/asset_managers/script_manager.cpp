#include <cathedral/editor/asset_managers/script_manager.hpp>

#include <QCloseEvent>

#include "ui_script_manager.h"

namespace cathedral::editor
{
    script_manager::script_manager(project::project* pro, engine::scene& scene, QWidget* parent, const bool allow_select)
        : QMainWindow(parent)
        , resource_manager_base(pro)
        , _ui(new Ui::script_manager)
        , _scene(scene)
        , _allow_select(allow_select)
    {
        _ui->setupUi(this);

        connect(_ui->actionClose, &QAction::triggered, this, [this] { close(); });
    }

    item_manager* script_manager::get_item_manager_widget()
    {
        return _ui->itemManagerWidget;
    }

    const item_manager* script_manager::get_item_manager_widget() const
    {
        return _ui->itemManagerWidget;
    }

    void script_manager::closeEvent(QCloseEvent* event)
    {
        if (!_modified_script_paths.empty())
        {
            if (show_confirm_dialog("Unsaved changes will be lost. Continue?", this))
            {
                emit closed();

                close();
                event->accept();
            }
            else
            {
                event->ignore();
            }
        }
        else
        {
            emit closed();

            close();
            event->accept();
        }
    }

    void script_manager::handle_new()
    {
        auto* diag = new text_input_dialog(this, "New script", "Name", false, "new_script");
        if (diag->exec() == QDialog::Accepted)
        {
            const auto name = diag->result_input();
            const auto path = _project->name_to_abspath<project::shader_asset>(name.toStdString());

            if (_project->shader_assets().contains(path))
            {
                show_error_message(QString{ "Script with name '" } + name + "' already exists");
                return;
            }

            auto new_asset = std::make_shared<project::dynamic_script_asset>(_project, path);
            new_asset->mark_as_manually_loaded();
            new_asset->save();

            _project->add_asset(new_asset);
            reload_item_list();

            const bool select_ok = _ui->itemManagerWidget->select_item(name);
            CRITICAL_CHECK(select_ok, "Failure selecting item");
        }
    }

    void script_manager::handle_rename()
    {
        const auto result = rename_asset();

        // If rename was successful, propagate renaming across dependent assets
        if (result.has_value())
        {
            const auto& [before, after] = *result;

            // Propagate rename into dependent materials
            for (const auto& asset : _project->material_assets() | std::views::values)
            {
                bool modified = false;
                if (asset->vertex_shader_ref() == before)
                {
                    asset->set_vertex_shader_ref(after);
                    modified = true;
                }
                if (asset->fragment_shader_ref() == before)
                {
                    asset->set_fragment_shader_ref(after);
                    modified = true;
                }

                if (modified)
                {
                    asset->save();
                }
            }
        }
    }

    void script_manager::handle_remove()
    {
    }

} // namespace cathedral::editor