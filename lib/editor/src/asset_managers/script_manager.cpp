#include <cathedral/editor/asset_managers/script_manager.hpp>

#include <cathedral/editor/asset_managers/script_syntax_highlighter.hpp>
#include <cathedral/editor/styling.hpp>
#include <cathedral/engine/node_filters.hpp>
#include <cathedral/engine/nodes/node.hpp>
#include <cathedral/engine/scene.hpp>

#include <QTimer>

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

        _code_editor = _ui->centralwidget;
        _code_editor->setEnabled(false);

        _highlighter = new script_syntax_highlighter(_code_editor->text_edit_widget()->document());

        _code_editor->text_edit_widget()->setTabStopDistance(
            QFontMetrics(_code_editor->text_edit_widget()->font()).horizontalAdvance(' ') * 4);
        _code_editor->text_edit_widget()->setStyleSheet("QPlainTextEdit{background-color: #D0D0D0;}");

        connect(_ui->actionClose, &QAction::triggered, this, [this] { close(); });
        connect(_ui->actionOpenInExternalEditor, &QAction::triggered, this, [this] { handle_open_in_external_editor(); });

        connect(_ui->itemManagerWidget, &item_manager::item_selection_changed, this, &SELF::handle_item_selection_changed);
        connect(_ui->itemManagerWidget, &item_manager::add_clicked, this, &SELF::handle_new);
        connect(_ui->itemManagerWidget, &item_manager::rename_clicked, this, &SELF::handle_rename);
        connect(_ui->itemManagerWidget, &item_manager::delete_clicked, this, &SELF::handle_delete);

        connect(_ui->pushButton_Save, &QPushButton::clicked, this, &SELF::handle_save);

        _import_timer = new QTimer(this);
        _import_timer->setInterval(1000);
        connect(_import_timer, &QTimer::timeout, this, [this] {

        });
        _import_timer->start();

        reload_item_list();
    }

    item_manager* script_manager::get_item_manager_widget()
    {
        return _ui->itemManagerWidget;
    }

    const item_manager* script_manager::get_item_manager_widget() const
    {
        return _ui->itemManagerWidget;
    }

    void script_manager::closeEvent([[maybe_unused]] QCloseEvent* event)
    {
        if (!_modified_script_paths.empty())
        {
            if (show_confirm_dialog("Unsaved changes will be lost. Continue?", this))
            {
                emit closed();
                close();
            }
        }
        else
        {
            emit closed();
            close();
        }
    }

    void script_manager::handle_item_selection_changed(const std::optional<QString>& selection)
    {
        const bool selected = selection.has_value();
        _ui->pushButton_Save->setEnabled(selected);
        _code_editor->setEnabled(selected);
        if (!selected)
        {
            _code_editor->set_text("");
            return;
        }

        const auto selected_text = _ui->itemManagerWidget->current_text();
        const auto path = _project->name_to_abspath<project::dynamic_script_asset>(selected_text.toStdString());
        auto asset = _project->get_asset_by_path<project::dynamic_script_asset>(path);

        const QString source = [this, asset] {
            if (!_temp_sources.contains(asset->name()))
            {
                _temp_sources[asset->name()] = QString::fromStdString(asset->source());
            }
            return _temp_sources[asset->name()];
        }();

        _code_editor->text_edit_widget()->blockSignals(true);
        _code_editor->text_edit_widget()->setPlainText(source);
        _code_editor->text_edit_widget()->blockSignals(false);
    }

    constexpr auto SCRIPT_INITIAL_SOURCE = R"(
-- function init(node, scene)
-- end

-- function editor_tick(node, scene, deltatime)
-- end

-- function tick(node, scene, deltatime)
-- end

-- function teardown(node, scene, deltatime)
-- end
)";

    void script_manager::handle_new()
    {
        auto* diag = new text_input_dialog(this, "New script", "Name", false, "new_script");
        if (diag->exec() == QDialog::Accepted)
        {
            const auto name = diag->result_input();
            const auto path = _project->name_to_abspath<project::dynamic_script_asset>(name.toStdString());

            if (_project->script_assets().contains(path))
            {
                show_error_message(QString{ "Script with name '" } + name + "' already exists");
                return;
            }

            const auto new_asset = std::make_shared<project::dynamic_script_asset>(_project, path);
            new_asset->mark_as_manually_loaded();
            new_asset->set_source(SCRIPT_INITIAL_SOURCE);
            new_asset->save();

            _project->add_asset(new_asset);
            reload_item_list();

            _code_editor->set_text(SCRIPT_INITIAL_SOURCE);

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
                }
            }

            bool nodes_modified = false;
            auto nodes = _scene.root_nodes();
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
                _scene.load_nodes(std::move(nodes));
            }
        }
    }

    void script_manager::handle_delete()
    {
        // If script is deleted, remove references from nodes
        if (const auto deleted_name = delete_asset())
        {
            _modified_script_paths.erase(*deleted_name);
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
                }
            }

            // Try reloading current scene nodes
            bool nodes_modified = false;
            auto nodes = _scene.root_nodes();
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
                _scene.load_nodes(std::move(nodes));
            }
        }
    }

    void script_manager::handle_save()
    {
        if (_ui->itemManagerWidget->current_text().isEmpty())
        {
            return;
        }

        const auto selected_path = _ui->itemManagerWidget->current_text();
        const auto source = _code_editor->text_edit_widget()->toPlainText();
        const auto name = selected_path.toStdString();

        const auto asset = _project->script_assets().at(name);
        asset->set_source(source.toStdString());
        asset->save();

        _project->reload_script_assets();

        _ui->itemManagerWidget->current_item()->setFont(get_editor_font());
        _modified_script_paths.erase(_ui->itemManagerWidget->current_text().toStdString());

        // Reload modified script from scene nodes
        for (const auto& scene_name : _project->available_scenes())
        {
            bool nodes_modified = false;
            auto nodes = _project->get_scene_nodes(scene_name);
            for (const auto& node : engine::flatten_node_tree(nodes) | engine::filter_nodes<engine::node>())
            {
                auto it = std::ranges::find(node->script_names(), name);
                if (it != node->script_names().end())
                {
                    node->remove_script(name);
                    node->add_script(name);
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

        // If current scene is not saved, reload scripts on nodes too
        bool nodes_modified = false;
        auto nodes = _scene.root_nodes();
        for (const auto& node : engine::flatten_node_tree(nodes) | engine::filter_nodes<engine::node>())
        {
            auto it = std::ranges::find(node->script_names(), name);
            if (it != node->script_names().end())
            {
                node->remove_script(name);
                node->add_script(name);
                nodes_modified = true;
            }
        }

        if (nodes_modified)
        {
            _scene.load_nodes(std::move(nodes));
        }
    }

    void script_manager::handle_open_in_external_editor()
    {
        const std::filesystem::path path(get_current_asset()->absolute_path());
        system(("vscodium " + path.parent_path().string()).c_str());
    }
} // namespace cathedral::editor