#pragma once

#include <QMainWindow>

namespace cathedral::project
{
    class material_definition_asset;
    class project;
}

namespace Ui
{
    class material_definition_manager;
}

namespace cathedral::editor
{
    class material_definition_manager : public QMainWindow
    {
        Q_OBJECT

    public:
        material_definition_manager(project::project& pro, QWidget* parent);

        void reload();

    private:
        project::project& _project;
        Ui::material_definition_manager* _ui = nullptr;

        void reload_variables(const project::material_definition_asset& asset);
        std::shared_ptr<project::material_definition_asset> get_current_asset();

    private slots:
        void slot_selected_changed();
        void slot_new_material_definition();
        void slot_save_clicked();
    };
} // namespace cathedral::editor