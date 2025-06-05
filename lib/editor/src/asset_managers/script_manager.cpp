#include <cathedral/editor/asset_managers/script_manager.hpp>

#include <QCloseEvent>

#include "ui_script_manager.h"

namespace cathedral::editor
{
    script_manager::script_manager(project::project* pro, engine::scene& scene, QWidget* parent, const bool allow_select)
        : QMainWindow(parent)
        , resource_manager_base(pro)
        , _ui(new Ui::script_manager)
        , _allow_select(allow_select)
        , _scene(scene)
    {
        _ui->setupUi(this);
    }

    item_manager* script_manager::get_item_manager_widget()
    {
        return _ui->itemManagerWidget;
    }

    const item_manager* script_manager::get_item_manager_widget() const
    {
        return _ui->itemManagerWidget;
    }

    void script_manager::closeEvent(QCloseEvent* ev)
    {
        if (!_modified_script_paths.empty())
        {
            if (show_confirm_dialog("Unsaved changes will be lost. Continue?", this))
            {
                emit closed();

                close();
                ev->accept();
            }
            else
            {
                ev->ignore();
            }
        }
        else
        {
            emit closed();

            close();
            ev->accept();
        }
    }

} // namespace cathedral::editor