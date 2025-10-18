#pragma once

#include "cathedral/editor2/settings.hpp"

#include <cathedral/core.hpp>

#include <functional>
#include <unordered_set>

FORWARD_CLASS(cathedral::engine, scene);
FORWARD_CLASS(cathedral::engine, scene_node);

namespace cathedral::editor2
{
    class scene_tree
    {
    public:
        void tick(engine::scene& scene);

        const std::unordered_set<engine::scene_node*>& selected_nodes() const { return _selected_nodes; }

        static constexpr auto WINDOW_ID = "Scene tree";

    private:
        std::unordered_set<engine::scene_node*> _selected_nodes;
        bool _open_context_menu_flag = false;

        bool _rename_mode = false;
        std::string _rename_buffer;

        bool _reparent_mode = false;

        engine::scene_node* _force_expand_node = nullptr;

        void draw_node(engine::scene& scene, engine::scene_node&);
        void context_menu(engine::scene& scene);

        void delete_selected_nodes(engine::scene& scene);

        void new_root_node_menu(engine::scene& scene);
        void new_child_node_menu();

        void duplicate_selected_nodes(engine::scene& scene);
    };
} // namespace cathedral::editor2