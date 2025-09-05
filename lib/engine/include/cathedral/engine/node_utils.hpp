#pragma once

#include <cathedral/engine/scene_node.hpp>

#include <memory>
#include <vector>

namespace cathedral::engine
{
    namespace internal
    {
        struct node_tree_recurse
        {
            template <typename TNode = scene_node, typename TCallable>
            void apply(scene_node* node, const TCallable& callable) const
            {
                static_assert(
                    std::is_invocable_v<TCallable, TNode*>,
                    "Callable object requires being callable with an argument of type 'scene_node*'");

                if constexpr (std::is_same_v<TNode, scene_node>)
                {
                    callable(node);
                    for (const auto& child : node->children())
                    {
                        apply<TNode>(child.get(), callable);
                    }
                }
                else
                {
                    if (auto* cast_node = dynamic_cast<TNode*>(node))
                    {
                        callable(cast_node);
                    }

                    for (const auto& child : node->children())
                    {
                        if (auto* cast_node = dynamic_cast<TNode*>(child.get()))
                        {
                            apply<TNode>(cast_node, callable);
                        }
                    }
                }
            }
        };
    } // namespace internal

    template <typename TNode = scene_node, typename TCallable>
    void recurse_node_tree(scene_node* node, const TCallable& callable)
    {
        internal::node_tree_recurse{}.apply<TNode>(node, callable);
    }

    template <typename TNode = scene_node, typename TCallable>
    void recurse_node_trees(const std::vector<std::unique_ptr<scene_node>>& nodes, const TCallable& callable)
    {
        for (const auto& node : nodes)
        {
            internal::node_tree_recurse{}.apply<TNode>(node.get(), callable);
        }
    }
} // namespace cathedral::engine
