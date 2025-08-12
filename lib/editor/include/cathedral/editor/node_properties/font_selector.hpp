#pragma once

#include <cathedral/core.hpp>

#include <cathedral/project/assets/font_asset.hpp>

#include <QWidget>

FORWARD_CLASS_INLINE(QLabel);
FORWARD_CLASS(cathedral::engine, scene);

namespace cathedral::editor
{
    class font_selector final : public QWidget
    {
        Q_OBJECT

    public:
        font_selector(project::project* project, engine::scene& scene, QWidget* parent, const QString& initial_text = "");
        void set_text(const QString& text) const;

    protected:
        project::project* _project;
        engine::scene& _scene;
        QLabel* _text = nullptr;

    signals:
        void font_selected(std::shared_ptr<project::font_asset> asset);

    protected:
        void open_select_dialog();
    };
} // namespace cathedral::editor