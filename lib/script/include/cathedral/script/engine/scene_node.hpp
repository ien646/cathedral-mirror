#pragma once

#include <cathedral/script/state.hpp>

namespace cathedral::script::engine
{
    struct scene_node_initializer final : state_initializer
    {
        void initialize(state& s) override;
    };
} // namespace cathedral::script::engine