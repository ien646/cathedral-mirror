#pragma once

#include <cathedral/editor/scene_tree.hpp>
#include <cathedral/engine/scene.hpp>

#include <QDockWidget>

class QTreeWidgetItem;

namespace cathedral::editor
{
    class scene_dock_widget : public QDockWidget
    {
        Q_OBJECT

    public:
        scene_dock_widget(QWidget* parent = nullptr);
        void set_scene(engine::scene* scn);

    private:
        scene_tree* _tree = nullptr;
        engine::scene* _scene = nullptr;

        void process_node(QTreeWidgetItem* parent_widget, const engine::scene_node& scene_node, const std::string& name);

    signals:
        void node_selected(engine::scene_node* node);
    };
} // namespace cathedral::editor