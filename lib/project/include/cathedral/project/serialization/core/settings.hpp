#pragma once

#include <cathedral/project/serialization/enums.hpp> //NOLINT
#include <cathedral/settings.hpp>

#include <cereal/cereal.hpp>

namespace cereal
{
    template <typename Archive>
    void CEREAL_SAVE_FUNCTION_NAME(Archive& ar, const cathedral::setting_value& value)
    {
        std::visit(
            [&](const auto& variant_value) { ar(make_nvp("type", value.type()), make_nvp("value", variant_value)); },
            value.get());
    }

    template <typename Archive>
    void CEREAL_LOAD_FUNCTION_NAME(Archive& ar, cathedral::setting_value& value)
    {
        cathedral::setting_type type;
        ar(type);

        switch (type)
        {
        case cathedral::setting_type::INT64: {
            int64_t v;
            ar(v);
            value.set(v);
            break;
        }
        case cathedral::setting_type::DOUBLE: {
            double v;
            ar(v);
            value.set(v);
            break;
        }
        case cathedral::setting_type::STRING: {
            std::string v;
            ar(v);
            value.set(v);
            break;
        }
        default:
            break;
        }
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

        for (const auto& [k, v] : entries)
        {
            value.set(k, v);
        }
    }
} // namespace cereal