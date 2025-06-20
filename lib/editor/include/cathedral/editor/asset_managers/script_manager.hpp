#pragma once

#include <cathedral/core.hpp>

#include <cathedral/editor/asset_managers/resource_manager_base.hpp>

#include <QMainWindow>

FORWARD_CLASS(cathedral::editor, code_editor);
FORWARD_CLASS(cathedral::editor, script_syntax_highlighter);
FORWARD_CLASS(cathedral::project, dynamic_script_asset);
FORWARD_CLASS(cathedral::project, project);
FORWARD_CLASS(Ui, script_manager); // NOLINT

namespace cathedral::editor
{
    class script_manager final
        : public QMainWindow
        , public resource_manager_base<project::dynamic_script_asset>
    {
        Q_OBJECT

    public:
        script_manager(project::project* pro, engine::scene& scene, QWidget* parent, bool allow_select = false);

        item_manager* get_item_manager_widget() override;
        const item_manager* get_item_manager_widget() const override;

    signals:
        void closed();

    private:
        Ui::script_manager* _ui = nullptr;
        code_editor* _code_editor = nullptr;
        script_syntax_highlighter* _highlighter = nullptr;
        engine::scene& _scene;
        bool _allow_select = false;

        std::unordered_set<std::string> _modified_script_paths;
        std::unordered_map<std::string, QString> _temp_sources;

        void closeEvent(QCloseEvent* event) override;

        void handle_item_selection_changed(const std::optional<QString>& selected);
        void handle_new();
        void handle_rename();
        void handle_delete();

        void handle_save();

        void handle_open_in_external_editor();
    };
} // namespace cathedral::editor