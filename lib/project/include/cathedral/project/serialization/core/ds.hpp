#pragma once

#include <cathedral/ds.hpp>

#include <cereal/macros.hpp>

namespace cereal
{
    template <typename Archive, typename... TArgs>
    void CEREAL_LOAD_FUNCTION_NAME(Archive& ar, cathedral::unordered_map<TArgs...>& umap)
    {
        std::unordered_map<TArgs...> intermediate;
        ar(intermediate);

        umap = { intermediate.begin(), intermediate.end() };
    }

    template <typename Archive, typename... TArgs>
    void CEREAL_SAVE_FUNCTION_NAME(Archive& ar, const cathedral::unordered_map<TArgs...>& umap)
    {
        const std::unordered_map<TArgs...> intermediate = { umap.begin(), umap.end() };
        ar(intermediate);
    }
} // namespace cereal