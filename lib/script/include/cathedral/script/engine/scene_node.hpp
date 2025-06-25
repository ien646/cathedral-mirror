#pragma once

#include <cathedral/script/state.hpp>

namespace cathedral::script::engine
{
    struct scene_node_initializer final : state_initializer
    {
        void initialize(state& s) override;
        const std::string& get_annotations() override;
        const std::string& get_inheritable_annotations() override;
    };
} // namespace cathedral::script::engine