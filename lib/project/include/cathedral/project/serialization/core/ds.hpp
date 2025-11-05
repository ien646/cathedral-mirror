#pragma once

#include <cathedral/core.hpp>
#include <cathedral/ds.hpp>

namespace cathedral::project
{
    template <typename T>
    auto to_std_unordered_map(const T& map)
    {
        return std::unordered_map(map.begin(), map.end());
    }
} // namespace cathedral::project