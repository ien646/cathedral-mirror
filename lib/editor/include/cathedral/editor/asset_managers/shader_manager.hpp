#pragma once

#include <QWidget>

#include <cathedral/project/project.hpp>

namespace Ui
{
    class shader_manager;
}

namespace cathedral::editor
{
    class shader_manager : public QWidget
    {
        Q_OBJECT
        
    public:
        shader_manager(project::project& pro);

        void reload();

    private:
        project::project& _project;
        Ui::shader_manager* _ui = nullptr;

        gfx::shader_type get_shader_type() const;
        std::shared_ptr<project::shader_asset> get_shader_asset_by_path(const std::string& path) const;

    private slots:
        void slot_selected_shader_changed();
        void slot_add_shader_clicked();
        void slot_validate_clicked();
        void slot_save_clicked();
        void slot_rename_clicked();
    };
}