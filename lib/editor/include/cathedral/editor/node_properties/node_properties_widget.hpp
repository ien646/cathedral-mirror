#pragma once

#include <cathedral/core.hpp>

#include <QWidget>

FORWARD_CLASS_INLINE(QVBoxLayout);
FORWARD_CLASS(cathedral::project, project);
FORWARD_CLASS(cathedral::engine, node);

namespace cathedral::editor
{
    FORWARD_CLASS_INLINE(transform_widget);

    class node_properties_widget : public QWidget
    {
    public:
        node_properties_widget(project::project* pro, QWidget* parent, engine::node* node, bool addStretch);

    protected:
        project::project* _project;
        QVBoxLayout* _main_layout = nullptr;
        transform_widget* _transform_widget = nullptr;

        void init_ui(bool addStretch) const;
        void update_transform_widget() const;

    private:
        engine::node* _node = nullptr;
    };
} // namespace cathedral::editor