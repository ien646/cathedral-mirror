#pragma once

#include <cathedral/engine/scene.hpp>

#include <QTreeWidget>

#include <unordered_map>
#include <unordered_set>

class QTimer;

namespace cathedral::editor
{
    class scene_tree : public QTreeWidget
    {
        Q_OBJECT

    public:
        scene_tree(QWidget* parent);

        void set_scene(engine::scene* scene);

    protected:
        void paintEvent(QPaintEvent* ev) override;
        void mousePressEvent(QMouseEvent* ev) override;
        void mouseReleaseEvent(QMouseEvent* ev) override;

    private:
        engine::scene* _scene = nullptr;
        std::weak_ptr<engine::scene_node> _selected_node;
        std::unordered_set<std::string> _expanded_nodes;
        std::unordered_map<engine::scene_node*, QTreeWidgetItem*> _node_to_item;
        std::unordered_map<QTreeWidgetItem*, engine::scene_node*> _item_to_node;

        QTimer* _refresh_timer = nullptr;

        void update_tree();
        void process_node(QTreeWidgetItem* parent_widget, engine::scene_node& scene_node, const std::string& name);

        QTreeWidgetItem* get_tree_item_for_node(engine::scene_node* node);
        std::shared_ptr<engine::scene_node> get_node_for_tree_item(QTreeWidgetItem* item);

        void handle_custom_context_menu_request(const QPoint& pos);
        std::vector<std::string> get_node_route_at_position(const QPoint& pos) const;

        void handle_add_node(const std::vector<std::string>& route);
        void handle_rename_node(const std::vector<std::string>& route);

    signals:
        void node_selected(engine::scene_node* node);
    };
} // namespace cathedral::editor