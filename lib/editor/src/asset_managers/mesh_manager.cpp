#include <cathedral/editor/asset_managers/mesh_manager.hpp>

#include <cathedral/editor/asset_managers/dialogs/new_mesh_dialog.hpp>
#include <cathedral/editor/common/mesh_viewer.hpp>
#include <cathedral/engine/node_filters.hpp>
#include <cathedral/engine/node_utils.hpp>
#include <cathedral/engine/nodes/mesh3d_node.hpp>
#include <cathedral/engine/scene.hpp>
#include <cathedral/project/assets/mesh_asset.hpp>

#include "ui_mesh_manager.h"

#include <QTimer>

namespace cathedral::editor
{
    mesh_manager::mesh_manager(project::project* pro, engine::scene& scene, QWidget* parent, bool allow_select)
        : QMainWindow(parent)
        , resource_manager_base(pro)
        , _ui(new Ui::mesh_manager)
        , _scene(scene)
        , _allow_select(allow_select)
    {
        setObjectName("mesh_manager");

        _ui->setupUi(this);

        show();

        _ui->mesh_viewer->initialize(_project, std::nullopt);

        connect(_ui->actionClose, &QAction::triggered, this, &SELF::close);
        connect(_ui->item_manager, &item_manager::add_clicked, this, &SELF::handle_add_mesh_clicked);
        connect(_ui->item_manager, &item_manager::rename_clicked, this, &SELF::handle_rename_mesh_clicked);
        connect(_ui->item_manager, &item_manager::delete_clicked, this, &SELF::handle_delete_mesh_clicked);
        connect(_ui->item_manager, &item_manager::item_selection_changed, this, &SELF::handle_mesh_selection_changed);

        if (_allow_select)
        {
            connect(_ui->pushButton_Select, &QPushButton::clicked, this, [this] {
                emit mesh_selected(get_current_asset());
                close();
            });
            connect(_ui->pushButton_Cancel, &QPushButton::clicked, this, [this] { close(); });
        }
        else
        {
            delete _ui->pushButton_Cancel;
            delete _ui->pushButton_Select;
        }
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

    void mesh_manager::closeEvent([[maybe_unused]] QCloseEvent* ev)
    {
        // Delete mesh_viewer early so that the associated swapchain is destroyed before the surface
        delete _ui->mesh_viewer;
        _ui->mesh_viewer = nullptr;

        emit closed();
        QMainWindow::closeEvent(ev);
    }

    void mesh_manager::handle_add_mesh_clicked()
    {
        auto* diag = new new_mesh_dialog(_ui->item_manager->get_texts(), this);
        if (diag->exec() == QDialog::DialogCode::Accepted)
        {
            const auto& name = diag->name();
            const auto& path = diag->path();

            auto new_asset = std::make_shared<project::mesh_asset>(
                _project,
                _project->name_to_abspath<project::mesh_asset>(name.toStdString()));

            const engine::mesh mesh(path.toStdString());
            new_asset->save_mesh(mesh);
            new_asset->mark_as_manually_loaded();
            new_asset->save();

            _project->add_asset(std::move(new_asset));
        }
        reload_item_list();
    }

    void mesh_manager::handle_rename_mesh_clicked()
    {
        const auto result = rename_asset();

        // If rename was successful, propagate renaming across dependent assets
        if (result.has_value())
        {
            const auto& [before, after] = *result;

            const auto process_nodes = [&](auto& nodes) -> bool {
                bool modified = false;
                engine::recurse_node_trees<engine::mesh3d_node>(nodes, [&](const std::shared_ptr<engine::mesh3d_node>& mesh3d_node) {
                    for (uint32_t i = 0; i < mesh3d_node->texture_names().size(); ++i)
                    {
                        if (mesh3d_node->texture_names()[i] == before)
                        {
                            mesh3d_node->bind_node_texture_slot(after, i);
                            modified = true;
                        }
                    }
                });
                return modified;
            };

            // Replace renamed asset in dependent assets
            for (const auto& scene_name : _project->available_scenes())
            {
                auto nodes = _project->get_scene_root_nodes(scene_name);
                if (process_nodes(nodes))
                {
                    _project->replace_scene_nodes(scene_name, std::move(nodes));
                }
            }

            // Try reload scene nodes
            auto nodes = _scene.root_nodes();
            if (process_nodes(nodes))
            {
                _scene.load_nodes(std::move(nodes));
            }
        }
    }

    void mesh_manager::handle_delete_mesh_clicked()
    {
        // If texture is deleted, remove references from nodes
        if (const auto deleted_name = delete_asset())
        {
            for (const auto& scene_name : _project->available_scenes())
            {
                bool nodes_modified = false;
                auto root_nodes = _project->get_scene_root_nodes(scene_name);
                engine::recurse_node_trees<engine::mesh3d_node>(
                    root_nodes,
                    [&](const std::shared_ptr<engine::mesh3d_node>& mesh3d_node) {
                        if (mesh3d_node->mesh_name() == *deleted_name)
                        {
                            mesh3d_node->set_mesh(std::nullopt);
                            nodes_modified = true;
                        }
                    });

                if (nodes_modified)
                {
                    _project->replace_scene_nodes(scene_name, root_nodes);
                    if (_scene.name() == scene_name)
                    {
                        _scene.load_nodes(std::move(root_nodes));
                    }
                }
            }
        }
    }

    void mesh_manager::handle_mesh_selection_changed(const std::optional<QString>& selected) const
    {
        const bool item_selected = selected.has_value() && !selected.value().isEmpty();
        if (_allow_select)
        {
            _ui->pushButton_Select->setEnabled(item_selected);
        }

        if (!item_selected)
        {
            return;
        }

        const std::string name = selected->toStdString();
        _ui->mesh_viewer->set_mesh(name);
    }
} // namespace cathedral::editor
