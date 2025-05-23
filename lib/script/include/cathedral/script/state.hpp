#pragma once

#include <sol/sol.hpp>

namespace cathedral::script
{
    using state = sol::state;

    struct state_initializer
    {
        virtual ~state_initializer() = default;
        virtual void initialize(state& s);
    };

    state get_initial_state();
} // namespace cathedral::script