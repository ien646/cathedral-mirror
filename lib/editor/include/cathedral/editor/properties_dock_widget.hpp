#pragma once

#include <cathedral/core.hpp>

#include <QDockWidget>
#include <utility>

FORWARD_CLASS_INLINE(QScrollArea);
FORWARD_CLASS(cathedral::engine, camera2d_node);
FORWARD_CLASS(cathedral::engine, camera3d_node);
FORWARD_CLASS(cathedral::engine, mesh3d_node);
FORWARD_CLASS(cathedral::engine, node);
FORWARD_CLASS(cathedral::engine, scene);
FORWARD_CLASS(cathedral::project, project);

namespace cathedral::editor
{
    class properties_dock_widget : public QDockWidget
    {
    public:
        properties_dock_widget(project::project* pro, std::shared_ptr<engine::scene> scene, QWidget* parent = nullptr);

        void set_scene(std::shared_ptr<engine::scene> scene) { _scene = std::move(scene); }

        void clear_node();
        void set_node(engine::mesh3d_node* node);
        void set_node(engine::camera2d_node* node);
        void set_node(engine::camera3d_node* node);
        void set_node(engine::node* node);

    private:
        project::project* _project;
        std::shared_ptr<engine::scene> _scene;

        QScrollArea* _scroll_area = nullptr;
        QWidget* _properties_widget = nullptr;

        template <typename TWidget>
        void set_node_generic(TWidget* widget);
    };
} // namespace cathedral::editor