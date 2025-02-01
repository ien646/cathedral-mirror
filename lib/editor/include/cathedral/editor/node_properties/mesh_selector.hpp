#pragma once

#include <cathedral/project/assets/mesh_asset.hpp>

#include <QWidget>

class QLabel;

namespace cathedral::editor
{
    class mesh_selector : public QWidget
    {
        Q_OBJECT

    public:
        mesh_selector(project::project& project, QWidget* parent, const QString& initial_text = "");
        void set_text(const QString& text);

    protected:
        project::project& _project;
        QLabel* _text = nullptr;

    signals:
        void mesh_selected(std::shared_ptr<project::mesh_asset> asset);

    protected:
        void open_select_dialog();
    };
} // namespace cathedral::editor