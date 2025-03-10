#pragma once

#include <cathedral/project/assets/material_asset.hpp>

#include <QWidget>

class QLabel;

namespace cathedral::editor
{
    class material_selector : public QWidget
    {
        Q_OBJECT

    public:
        material_selector(project::project* project, QWidget* parent, const QString& initial_text = "");
        void set_text(const QString& text);

    protected:
        project::project* _project;
        QLabel* _text = nullptr;

    signals:
        void material_selected(std::shared_ptr<project::material_asset> asset);

    protected:
        void open_select_dialog();
    };
} // namespace cathedral::editor