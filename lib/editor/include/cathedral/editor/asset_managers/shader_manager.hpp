#pragma once

#include <cathedral/editor/asset_managers/resource_manager_base.hpp>

#include <cathedral/gfx/shader.hpp>

#include <cathedral/project/assets/shader_asset.hpp>

#include <QMainWindow>

#include <unordered_set>

FORWARD_CLASS(Ui, shader_manager); //NOLINT
FORWARD_CLASS(cathedral::engine, scene);
FORWARD_CLASS(cathedral::project, project);

namespace cathedral::editor
{
    class code_editor;
    class shader_syntax_highlighter;

    class shader_manager
        : public QMainWindow
        , public resource_manager_base<project::shader_asset>
    {
        Q_OBJECT

    public:
        shader_manager(project::project* pro, engine::scene& scene, QWidget* parent);

        item_manager* get_item_manager_widget() override;
        const item_manager* get_item_manager_widget() const override;

    signals:
        void closed();

    private:
        Ui::shader_manager* _ui = nullptr;
        engine::scene& _scene;
        code_editor* _code_editor = nullptr;
        shader_syntax_highlighter* _highlighter = nullptr;

        gfx::shader_type get_shader_type() const;

        std::unordered_set<std::string> _modified_shader_paths;
        std::unordered_map<std::string, QString> _temp_sources;

        void closeEvent(QCloseEvent* ev) override;
        void showEvent(QShowEvent* ev) override;

        void handle_selected_shader_changed();
        void handle_add_shader_clicked();
        void handle_show_processed_clicked();
        void handle_validate_clicked();
        void handle_save_clicked();
        void handle_rename_clicked();
        void handle_delete_clicked();
        void handle_text_edited();
        void handle_shader_template_insert_clicked() const;
    };
} // namespace cathedral::editor