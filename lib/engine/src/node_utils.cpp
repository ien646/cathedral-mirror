#include <cathedral/engine/node_utils.hpp>

namespace cathedral::engine
{
    namespace
    {
        void add_node_and_children_recursive(
            const std::shared_ptr<scene_node>& node,
            std::vector<std::shared_ptr<scene_node>>& target)
        {
            target.push_back(node);
            for (const auto& child : node->children())
            {
                add_node_and_children_recursive(child, target);
            }
        }
    } // namespace

    std::vector<std::shared_ptr<scene_node>> flatten_node_tree(const std::vector<std::shared_ptr<scene_node>>& node_tree)
    {
        std::vector<std::shared_ptr<scene_node>> result;
        for (const auto& node : node_tree)
        {
            add_node_and_children_recursive(node, result);
        }
        return result;
    }
}