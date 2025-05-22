#pragma once

#include <cathedral/script/state.hpp>

namespace cathedral::script
{
    struct scene_node_initializer : state_initializer
    {
        void initialize(state& s) override;
    };
} // namespace cathedral::script