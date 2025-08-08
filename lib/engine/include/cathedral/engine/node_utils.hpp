#pragma once

#include <cathedral/engine/scene_node.hpp>

#include <memory>
#include <vector>

namespace cathedral::engine
{
    [[deprecated("Use cathedral::engine::recurse_node_tree(s) instead")]]
    std::vector<std::shared_ptr<scene_node>> flatten_node_tree(const std::vector<std::shared_ptr<scene_node>>& node_tree);

    namespace internal
    {
        struct node_tree_recurse
        {
            template <typename TNode = scene_node, typename TCallable>
            void apply(const std::shared_ptr<scene_node>& node, const TCallable& callable) const
            {
                static_assert(
                    std::is_invocable_v<TCallable, const std::shared_ptr<TNode>&>,
                    "Callable object requires being callable with an argument of type 'const std::shared_ptr<TNode>&'");

                if constexpr (std::is_same_v<TNode, scene_node>)
                {
                    callable(node);
                    for (const auto& child : node->children())
                    {
                        apply<TNode>(child, callable);
                    }
                }
                else
                {
                    if (const auto& cast_node = std::dynamic_pointer_cast<TNode>(node))
                    {
                        callable(cast_node);
                    }
                    for (const auto& child : node->children())
                    {
                        apply<TNode>(child, callable);
                    }
                }
            }
        };
    } // namespace internal

    template <typename TNode = scene_node, typename TCallable>
    void recurse_node_tree(const std::shared_ptr<scene_node>& node, const TCallable& callable)
    {
        internal::node_tree_recurse{}.apply<TNode>(node, callable);
    }

    template <typename TNode = scene_node, typename TCallable>
    void recurse_node_trees(const std::vector<std::shared_ptr<scene_node>>& nodes, const TCallable& callable)
    {
        for (const auto& node : nodes)
        {
            internal::node_tree_recurse{}.apply<TNode>(node, callable);
        }
    }
} // namespace cathedral::engine
