#pragma once

#include <cathedral/core.hpp>

#include <QWidget>

FORWARD_CLASS_INLINE(QHBoxLayout);
FORWARD_CLASS_INLINE(QVBoxLayout);
FORWARD_CLASS(cathedral::project, project);
FORWARD_CLASS(cathedral::engine, node);
FORWARD_CLASS(cathedral::editor, script_selector);
FORWARD_CLASS(cathedral::editor, transform_widget);

namespace cathedral::editor
{
    class node_properties_widget : public QWidget
    {
    public:
        node_properties_widget(project::project* pro, QWidget* parent, engine::node* node, bool add_stretch);

        void update_transform_widget() const;

    protected:
        project::project* _project;
        QVBoxLayout* _main_layout = nullptr;
        QHBoxLayout* _status_cbox_layout = nullptr;
        transform_widget* _transform_widget = nullptr;
        script_selector* _script_selector = nullptr;

        void init_ui(bool add_stretch);

    private:
        engine::node* _node = nullptr;
    };
} // namespace cathedral::editor