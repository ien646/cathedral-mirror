#pragma once

#include <QMainWindow>

namespace cathedral::project
{
    class project;
}

namespace Ui
{
    class texture_manager;
}

namespace cathedral::editor
{
    class texture_manager : public QMainWindow
    {
        Q_OBJECT

    public:
        texture_manager(project::project& pro, QWidget* parent);

    private:
        Ui::texture_manager* _ui = nullptr;
        project::project& _project;
        QImage _current_image = {};

        void reload();

        void resizeEvent(QResizeEvent* ev) override;

    private slots:
        void slot_add_texture();
        void slot_rename_texture();
        void slot_delete_texture();
        void slot_selected_texture_changed();
    };
}