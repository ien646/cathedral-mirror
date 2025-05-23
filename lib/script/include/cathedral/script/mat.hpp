#pragma once
#include "state.hpp"

namespace cathedral::script
{
    struct mat_initializer final : state_initializer
    {
        void initialize(state& s) override;
    };
}