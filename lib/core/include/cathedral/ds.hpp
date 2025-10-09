#pragma once

#include <boost/unordered/unordered_map.hpp>

namespace cathedral
{
    template <typename... TArgs>
    using unordered_map = boost::unordered_map<TArgs...>;
}