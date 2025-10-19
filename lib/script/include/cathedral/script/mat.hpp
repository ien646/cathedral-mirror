#pragma once

#include <cathedral/script/state.hpp>

namespace cathedral::script
{
    struct mat_initializer final : state_initializer
    {
        void initialize(state& s) override;
        const std::string& get_annotations() override;
    };
} // namespace cathedral::script