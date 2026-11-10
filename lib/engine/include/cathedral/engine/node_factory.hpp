#pragma once

#include <cathedral/core.hpp>
#include <cathedral/engine/node_type.hpp>

#include <functional>
#include <memory>

FORWARD_CLASS(cathedral::engine, scene_object);

namespace cathedral::engine
{
    using node_ctor = std::function<std::unique_ptr<scene_object>()>;

    void register_node_constructor(node_type type, node_ctor ctor);
    std::unique_ptr<scene_object> construct_node(node_type type);
} // namespace cathedral::engine