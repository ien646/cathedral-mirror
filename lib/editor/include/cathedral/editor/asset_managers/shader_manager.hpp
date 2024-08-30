#pragma once

#include <cathedral/editor/asset_managers/resource_manager_base.hpp>

#include <cathedral/gfx/shader.hpp>

#include <cathedral/project/assets/shader_asset.hpp>

#include <QMainWindow>

#include <unordered_set>

namespace Ui
{
    class shader_manager;
}

namespace cathedral::project
{
    class project;
}

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
        shader_manager(project::project& pro, QWidget* parent);

        item_manager* get_item_manager_widget() override;

    private:
        Ui::shader_manager* _ui = nullptr;
        code_editor* _code_editor = nullptr;
        shader_syntax_highlighter* _highlighter = nullptr;

        gfx::shader_type get_shader_type() const;

        std::unordered_set<std::string> _modified_shader_paths;
        std::unordered_map<std::string, QString> _temp_sources;

        void closeEvent(QCloseEvent*) override;

    private slots:
        void slot_selected_shader_changed();
        void slot_add_shader_clicked();
        void slot_validate_clicked();
        void slot_save_clicked();
        void slot_rename_clicked();
        void slot_delete_clicked();
        void slot_text_edited();
    };
} // namespace cathedral::editor