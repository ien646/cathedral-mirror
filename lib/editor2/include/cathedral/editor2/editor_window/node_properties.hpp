#pragma once

#include <cathedral/ds.hpp>

FORWARD_CLASS(cathedral::engine, scene_node);

namespace cathedral::editor2
{
    class node_properties
    {
    public:
        void tick(const unordered_set<engine::scene_node*>& nodes);

        static constexpr auto WINDOW_ID = "Node properties";
    };
}