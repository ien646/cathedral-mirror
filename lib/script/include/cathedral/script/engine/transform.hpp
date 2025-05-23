#pragma once

#include <cathedral/script/state.hpp>

namespace cathedral::script::engine
{
    struct transform_initializer final : state_initializer
    {
        void initialize(state& s) override;
    };
}