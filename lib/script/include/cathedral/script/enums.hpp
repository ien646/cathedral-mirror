#pragma once

#include <cathedral/script/state.hpp>

namespace cathedral::script
{
    struct enums_initializer final : state_initializer
    {
        void initialize(state& s) override;
    };
}