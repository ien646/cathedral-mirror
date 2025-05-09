#pragma once

#include <cathedral/engine/nodes/point_light_node.hpp>

#include <cereal/types/memory.hpp>
#include <cereal/types/polymorphic.hpp>
#include <cereal/types/string.hpp>

#include <ranges>

namespace cereal
{
    template <typename Archive>
    void CEREAL_SERIALIZE_FUNCTION_NAME(Archive& ar, cathedral::engine::point_light_data& pldata)
    {
        ar(pldata.position, pldata.intensity, pldata.range, pldata.falloff_coefficient, pldata.color);
    }

    template <typename Archive>
    void CEREAL_SAVE_FUNCTION_NAME(Archive& ar, const cathedral::engine::point_light_node& node)
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
           make_nvp("transform", node.get_local_transform()),
           make_nvp("point_light_data", node.data()));
    }

    template <typename Archive>
    void CEREAL_LOAD_FUNCTION_NAME(Archive& ar, cathedral::engine::point_light_node& node)
    {
        std::string name;
        std::string type;
        bool enabled;
        std::vector<std::shared_ptr<cathedral::engine::scene_node>> children;
        cathedral::engine::transform transform;
        cathedral::engine::point_light_data pl_data;

        ar(name, type, enabled, children, transform, pl_data);

        CRITICAL_CHECK(type == node.typestr(), "Invalid node typestr");

        node.set_name(std::move(name));
        node.set_enabled(enabled);
        node.set_children(std::move(children));
        node.set_local_transform(transform);
        node.set_insensity(pl_data.intensity);
        node.set_range(pl_data.range);
        node.set_color(pl_data.color);
        node.set_falloff_coefficient(pl_data.falloff_coefficient);
    }
} // namespace cereal

CEREAL_REGISTER_TYPE(cathedral::engine::point_light_node);
CEREAL_REGISTER_POLYMORPHIC_RELATION(cathedral::engine::scene_node, cathedral::engine::point_light_node);