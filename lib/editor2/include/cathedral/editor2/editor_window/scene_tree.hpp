#pragma once

#include <cathedral/core.hpp>

#include <functional>
#include <unordered_set>

FORWARD_CLASS(cathedral::engine, scene);
FORWARD_CLASS(cathedral::engine, scene_node);

namespace cathedral::editor2
{
    struct scene_tree_callbacks
    {
        std::function<void(const std::unordered_set<engine::scene_node*>& selected_nodes)> node_selection_changed;
        std::function<void()> context_menu_requested;
    };

    class scene_tree
    {
    public:
        void tick(engine::scene& scene);

        scene_tree_callbacks callbacks;

    private:
        std::unordered_set<engine::scene_node*> _selected_nodes;
        bool _open_context_menu_flag = false;

        bool _rename_mode = false;
        std::string _rename_buffer;

        bool _reparent_mode = false;

        engine::scene_node* _force_expand_node = nullptr;

        void draw_node(engine::scene& scene,engine::scene_node&);
        void context_menu(engine::scene& scene);

        void delete_selected_nodes(engine::scene& scene) const;

        void new_root_node_menu(engine::scene& scene);
        void new_child_node_menu();
    };
} // namespace cathedral::editor2