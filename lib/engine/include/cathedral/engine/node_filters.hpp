#pragma once

#include <cathedral/engine/scene_node.hpp>

#include <ranges>

namespace cathedral::engine
{
    template <typename TNode>
    auto filter_nodes()
    {
        return std::views::transform(
                   [](const std::shared_ptr<scene_node>& node) { return std::dynamic_pointer_cast<TNode>(node); }) |
               std::views::filter([](const std::shared_ptr<TNode>& node) { return node != nullptr; });
    }
} // namespace cathedral::engine