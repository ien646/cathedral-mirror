#pragma once

#include <cathedral/engine/nodes/node.hpp>

#include <cathedral/cereal_serializers.hpp>

namespace cathedral::project
{
    template<typename Archive>
    void CEREAL_SAVE_FUNCTION_NAME(Archive& ar, const engine::node& node)
    {
        ar(
            cereal::make_nvp("name", node.name()),
            cereal::make_nvp("enabled", node.enabled()),
            cereal::make_nvp("children", node.children())
        );
    }

    template<typename Archive>
    void CEREAL_LOAD_FUNCTION_NAME(Archive& ar, engine::node& node)
    {
        
    }
}