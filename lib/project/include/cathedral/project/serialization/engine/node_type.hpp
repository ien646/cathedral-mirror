#pragma once

#include <cathedral/engine/node_type.hpp>

#include <cereal/cereal.hpp>

namespace cereal
{
    template <typename Archive>
    void CEREAL_SAVE_FUNCTION_NAME(Archive& ar, const cathedral::engine::node_type& v)
    {
        ar(make_nvp("node_type", v.string_view()));
    }

    template <typename Archive>
    void CEREAL_LOAD_FUNCTION_NAME(Archive& ar, cathedral::engine::node_type& v)
    {
        std::string s;
        ar(s);
        v.replace(s);
    }
} // namespace cereal