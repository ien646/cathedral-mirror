#pragma once

#include <cathedral/script/state.hpp>

namespace cathedral::script
{
    struct log_initializer final : state_initializer
    {
        void initialize(state& s) override;
        const std::string& get_annotations() override;
    };
}