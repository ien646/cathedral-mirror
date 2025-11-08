#pragma once

#define CATHEDRAL_USE_BOOST_UNORDERED_DEBUG

#if defined(NDEBUG) || defined(CATHEDRAL_USE_BOOST_UNORDERED_DEBUG)
    #include <boost/unordered/unordered_map.hpp>
    #include <boost/unordered/unordered_set.hpp>
#else
    #include <unordered_map>
    #include <unordered_set>
#endif

namespace cathedral
{
#if defined(NDEBUG) || defined(CATHEDRAL_USE_BOOST_UNORDERED_DEBUG)
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