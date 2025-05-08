#pragma once

#include <cathedral/core.hpp>

#include <cathedral/project/assets/texture_asset.hpp>

#include <QWidget>

FORWARD_CLASS_INLINE(QLabel);

namespace cathedral::editor
{
    class texture_selector : public QWidget
    {
        Q_OBJECT

    public:
        texture_selector(project::project* project, QWidget* parent, const QString& initial_text = "");
        void set_text(const QString& text) const;

    protected:
        project::project* _project;
        QLabel* _text = nullptr;

    signals:
        void texture_selected(std::shared_ptr<project::texture_asset> asset);

    protected:
        void open_select_dialog();
    };
} // namespace cathedral::editor