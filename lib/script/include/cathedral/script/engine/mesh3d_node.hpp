#pragma once

#include <cathedral/core.hpp>

#include <cathedral/script/state.hpp>

namespace cathedral::script::engine
{
    struct mesh3d_node_initializer final : state_initializer
    {
        void initialize(state& s) override;
    };
}