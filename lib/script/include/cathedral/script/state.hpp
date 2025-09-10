#pragma once

#include <sol/sol.hpp>

namespace cathedral::script
{
    using state = sol::state;

    struct state_initializer
    {
        virtual ~state_initializer() = default;
        virtual void initialize(state& s) = 0;
        virtual const std::string& get_annotations() = 0;
    };

    state get_initial_state();
    std::string get_annotations();
} // namespace cathedral::script