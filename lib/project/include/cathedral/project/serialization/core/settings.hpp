#pragma once

#include <cathedral/settings.hpp>

#include <cereal/cereal.hpp>
#include <cereal/types/common.hpp>
#include <cereal/types/string.hpp>
#include <cereal/types/unordered_map.hpp>

namespace cereal
{
    template <typename Archive>
    void CEREAL_SAVE_FUNCTION_NAME(Archive& ar, const cathedral::setting_value& value)
    {
        ar(make_nvp("value", value.get()));
    }

    template <typename Archive>
    void CEREAL_LOAD_FUNCTION_NAME(Archive& ar, cathedral::setting_value& value)
    {
        cathedral::setting_value::variant_t variant;
        ar(variant);
        value.set(std::move(variant));
    }

    template <typename Archive>
    void CEREAL_SAVE_FUNCTION_NAME(Archive& ar, const cathedral::settings& value)
    {
        ar(make_nvp("entries", value.all_entries()));
    }

    template <typename Archive>
    void CEREAL_LOAD_FUNCTION_NAME(Archive& ar, cathedral::settings& value)
    {
        std::unordered_map<std::string, cathedral::setting_value> entries;
        ar(entries);

        for (const auto& [k, v] : value.all_entries())
        {
            value.set(k, v);
        }
    }
} // namespace cereal