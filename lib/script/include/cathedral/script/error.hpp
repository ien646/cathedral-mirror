#pragma once

#include <cathedral/script/state.hpp>

namespace cathedral::script
{
    struct error_initializer final : state_initializer
    {
        void initialize(state& s) override;
    };
} // namespace cathedral::script