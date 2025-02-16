#pragma once

#include <cathedral/engine/shader_variable.hpp>

#include <cathedral/cereal_serializers.hpp>

namespace cereal
{
    template <typename Archive>
    void CEREAL_SERIALIZE_FUNCTION_NAME(Archive& ar, cathedral::engine::shader_variable& var)
    {
        ar(cereal::make_nvp("name", var.name),
           cereal::make_nvp("type", var.type),
           cereal::make_nvp("count", var.count),
           cereal::make_nvp("binding", var.binding));
    }
} // namespace cereal