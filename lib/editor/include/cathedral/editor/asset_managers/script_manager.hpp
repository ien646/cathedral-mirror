#pragma once

#include <cathedral/core.hpp>

#include <cathedral/editor/asset_managers/resource_manager_base.hpp>

#include <QMainWindow>

FORWARD_CLASS(cathedral::project, dynamic_script_asset);
FORWARD_CLASS(cathedral::project, project);
FORWARD_CLASS(Ui, script_manager); // NOLINT

namespace cathedral::editor
{
    class script_manager
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
        engine::scene& _scene;
        bool _allow_select = false;

        std::unordered_set<std::string> _modified_script_paths;

        void closeEvent(QCloseEvent* event) override;

        void handle_new();
        void handle_rename();
        void handle_remove();
    };
} // namespace cathedral::editor