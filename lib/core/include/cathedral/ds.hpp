#pragma once

#ifdef NDEBUG
    #include <boost/unordered/unordered_map.hpp>
    #include <boost/unordered/unordered_set.hpp>
#else
    #include <unordered_map>
    #include <unordered_set>
#endif

namespace cathedral
{
#ifdef NDEBUG
    template <typename... TArgs>
    using unordered_map = boost::unordered_map<TArgs...>;

    template <typename... TArgs>
    using unordered_set = boost::unordered_set<TArgs...>;
#else
    template <typename... TArgs>
    using unordered_map = std::unordered_map<TArgs...>;

    template <typename... TArgs>
    using unordered_set = std::unordered_set<TArgs...>;
#endif
} // namespace cathedral