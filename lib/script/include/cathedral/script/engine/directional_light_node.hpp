#pragma once

#include <cathedral/script/state.hpp>

namespace cathedral::script::engine
{
    struct directional_light_node_initializer final : state_initializer
    {
        void initialize(state& s) override;
    };
}