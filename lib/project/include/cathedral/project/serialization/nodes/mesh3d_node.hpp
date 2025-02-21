#pragma once

#include <cathedral/engine/nodes/mesh3d_node.hpp>

#include <cathedral/cereal_serializers.hpp>

namespace cereal
{
    template <typename Archive>
    void CEREAL_SAVE_FUNCTION_NAME(Archive& ar, const cathedral::engine::mesh3d_node& node)
    {
        ar(cereal::make_nvp("not implemented", ""));
    }

    template <typename Archive>
    void CEREAL_LOAD_FUNCTION_NAME(Archive& ar, cathedral::engine::mesh3d_node& node)
    {
    }
} // namespace cereal

CEREAL_REGISTER_TYPE(cathedral::engine::mesh3d_node);
CEREAL_REGISTER_POLYMORPHIC_RELATION(cathedral::engine::scene_node, cathedral::engine::mesh3d_node);