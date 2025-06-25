#pragma once

#include <cathedral/script/state.hpp>

namespace cathedral::script::engine
{
    struct node_initializer final : state_initializer
    {
        void initialize(state& s) override;
        const std::string& get_annotations() override;
        const std::string& get_inheritable_annotations() override;
    };
}