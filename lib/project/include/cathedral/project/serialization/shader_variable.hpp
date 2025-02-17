#pragma once

#include <cathedral/engine/shader_variable.hpp>

#include <cathedral/cereal_serializers.hpp>

#include <cathedral/project/serialization/enums.hpp>

namespace cereal
{
    template <typename Archive>
    void CEREAL_SAVE_FUNCTION_NAME(Archive& ar, const cathedral::engine::shader_variable& var)
    {
        ar(cereal::make_nvp("name", var.name),
           cereal::make_nvp("type", std::string{ magic_enum::enum_name(var.type) }),
           cereal::make_nvp("count", var.count),
           cereal::make_nvp("binding", var.binding));
    }

    template <typename Archive>
    void CEREAL_LOAD_FUNCTION_NAME(Archive& ar, cathedral::engine::shader_variable& var)
    {
        std::string type_str;
        ar(var.name, type_str, var.count, var.binding);
        const auto type_opt = magic_enum::enum_cast<cathedral::gfx::shader_data_type>(type_str);
        CRITICAL_CHECK(type_opt.has_value());
        var.type = *type_opt;
    }
} // namespace cereal