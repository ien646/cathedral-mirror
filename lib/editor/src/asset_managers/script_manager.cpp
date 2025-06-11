#include <cathedral/editor/asset_managers/script_manager.hpp>

#include <cathedral/engine/scene.hpp>

#include "cathedral/engine/node_filters.hpp"
#include "cathedral/engine/nodes/node.hpp"
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

            const auto new_asset = std::make_shared<project::dynamic_script_asset>(_project, path);
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

            // Replace renamed asset in dependent assets
            for (const auto& scene_name : _project->available_scenes())
            {
                bool nodes_modified = false;
                auto nodes = _project->get_scene_nodes(scene_name);
                for (const auto& node : engine::flatten_node_tree(nodes) | engine::filter_nodes<engine::node>())
                {
                    auto it = std::ranges::find(node->script_names(), before);
                    if (it != std::ranges::end(node->script_names()))
                    {
                        node->remove_script(before);
                        node->add_script(after);
                        nodes_modified = true;
                    }
                }

                if (nodes_modified)
                {
                    _project->replace_scene_nodes(scene_name, nodes);
                    if (_scene.name() == scene_name)
                    {
                        _scene.load_nodes(std::move(nodes));
                    }
                }
            }
        }
    }

    void script_manager::handle_delete()
    {
        // If script is deleted, remove references from nodes
        if (const auto deleted_name = delete_asset())
        {
            for (const auto& scene_name : _project->available_scenes())
            {
                bool nodes_modified = false;
                auto nodes = _project->get_scene_nodes(scene_name);
                for (const auto& node : engine::flatten_node_tree(nodes) | engine::filter_nodes<engine::node>())
                {
                    auto it = std::ranges::find(node->script_names(), *deleted_name);
                    if (it != std::ranges::end(node->script_names()))
                    {
                        node->remove_script(*deleted_name);
                        nodes_modified = true;
                    }
                }

                if (nodes_modified)
                {
                    _project->replace_scene_nodes(scene_name, nodes);
                    if (_scene.name() == scene_name)
                    {
                        _scene.load_nodes(std::move(nodes));
                    }
                }
            }
        }
    }
} // namespace cathedral::editor