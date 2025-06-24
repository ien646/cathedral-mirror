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
        ar(make_nvp("node", cereal::base_class<cathedral::engine::node>(&node)), make_nvp("point_light_data", node.data()));
    }

    template <typename Archive>
    void CEREAL_LOAD_FUNCTION_NAME(Archive& ar, cathedral::engine::point_light_node& node)
    {
        cathedral::engine::point_light_data pl_data;

        ar(make_nvp("node", cereal::base_class<cathedral::engine::node>(&node)), pl_data);

        node.set_intensity(pl_data.intensity);
        node.set_range(pl_data.range);
        node.set_color(pl_data.color);
        node.set_falloff_coefficient(pl_data.falloff_coefficient);
    }
} // namespace cereal

CEREAL_REGISTER_TYPE(cathedral::engine::point_light_node);
CEREAL_REGISTER_POLYMORPHIC_RELATION(cathedral::engine::scene_node, cathedral::engine::point_light_node);