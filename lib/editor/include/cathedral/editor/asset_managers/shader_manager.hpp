#pragma once

#include <QMainWindow>

#include <cathedral/project/project.hpp>

#include <unordered_set>

namespace Ui
{
    class shader_manager;
}

namespace cathedral::editor
{
    class code_editor;
    class shader_syntax_highlighter;
    
    class shader_manager : public QMainWindow
    {
        Q_OBJECT
        
    public:
        shader_manager(project::project& pro);

        void reload();

    private:
        project::project& _project;
        Ui::shader_manager* _ui = nullptr;
        code_editor* _code_editor = nullptr;
        shader_syntax_highlighter* _highlighter = nullptr;

        gfx::shader_type get_shader_type() const;
        std::shared_ptr<project::shader_asset> get_shader_asset_by_path(const std::string& path) const;

        std::unordered_set<std::string> _modified_shader_paths;

    private slots:
        void slot_selected_shader_changed();
        void slot_add_shader_clicked();
        void slot_validate_clicked();
        void slot_save_clicked();
        void slot_rename_clicked();
        void slot_text_edited();
    };
}