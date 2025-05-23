#pragma once

#include <cathedral/engine/nodes/node.hpp>

#include <cereal/types/memory.hpp>
#include <cereal/types/polymorphic.hpp>
#include <cereal/types/string.hpp>

#include <ranges>

namespace cereal
{
    template <typename Archive>
    void CEREAL_SAVE_FUNCTION_NAME(Archive& ar, const cathedral::engine::node& node)
    {
        // Filter out editor nodes
        auto child_nodes_range = node.children() |
                                 std::views::filter([](const std::shared_ptr<cathedral::engine::scene_node>& child) {
                                     return !child->name().starts_with("__");
                                 });
        const std::vector<std::shared_ptr<cathedral::engine::scene_node>> children = { child_nodes_range.begin(),
                                                                                       child_nodes_range.end() };

        ar(make_nvp("name", node.name()),
           make_nvp("type", std::string{ node.typestr() }),
           make_nvp("enabled", node.enabled()),
           make_nvp("children", children),
           make_nvp("transform", node.get_local_transform()));
    }

    template <typename Archive>
    void CEREAL_LOAD_FUNCTION_NAME(Archive& ar, cathedral::engine::node& node)
    {
        std::string name;
        std::string type;
        bool enabled;
        std::vector<std::shared_ptr<cathedral::engine::scene_node>> children;
        cathedral::engine::transform transform;

        ar(name, type, enabled, children, transform);

        CRITICAL_CHECK(type == node.typestr(), "Invalid node typestr");

        node.set_name(std::move(name));
        node.set_enabled(enabled);
        node.set_children(std::move(children));
        node.set_local_transform(transform);
    }
} // namespace cereal

CEREAL_REGISTER_TYPE(cathedral::engine::node);
CEREAL_REGISTER_POLYMORPHIC_RELATION(cathedral::engine::scene_node, cathedral::engine::node);