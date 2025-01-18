#pragma once

#include <nlohmann/detail/macro_scope.hpp>
#include <nlohmann/json_fwd.hpp>

#include <type_traits>

namespace cathedral
{
    class serializable
    {
    public:
        virtual nlohmann::json to_json() const = 0;
        virtual void from_json(const nlohmann::json& json) = 0;
    };
} // namespace cathedral