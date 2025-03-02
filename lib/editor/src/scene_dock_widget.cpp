#include <cathedral/editor/scene_dock_widget.hpp>

#include <cathedral/editor/common/dock_title.hpp>

#include <cathedral/engine/nodes/mesh3d_node.hpp>

#include <QTreeWidget>

namespace cathedral::editor
{
    scene_dock_widget::scene_dock_widget(QWidget* parent)
        : QDockWidget(parent)
    {
        setTitleBarWidget(new dock_title("Scene Tree", this));

        _tree = new scene_tree(this);
        _tree->setHeaderHidden(true);
        setWidget(_tree);

        connect(_tree, &scene_tree::node_selected, this, [this](engine::scene_node* node) { emit node_selected(node); });
    }

    void scene_dock_widget::set_scene(engine::scene* scn)
    {
        _scene = scn;
        _tree->set_scene(_scene);
        update();
    }
} // namespace cathedral::editor