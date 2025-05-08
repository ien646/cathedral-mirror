#pragma once

#include <cathedral/editor/scene_tree.hpp>
#include <cathedral/engine/scene.hpp>

#include <QDockWidget>

FORWARD_CLASS_INLINE(QTreeWidgetItem);

namespace cathedral::editor
{
    class scene_dock_widget final : public QDockWidget
    {
        Q_OBJECT

    public:
        explicit scene_dock_widget(QWidget* parent = nullptr);
        void set_scene(engine::scene* scn);

        void reload() const;

    signals:
        void node_selected(cathedral::engine::scene_node* node);

    private:
        scene_tree* _tree = nullptr;
        engine::scene* _scene = nullptr;
    };
} // namespace cathedral::editor