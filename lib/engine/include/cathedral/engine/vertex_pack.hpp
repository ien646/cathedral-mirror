#pragma once

#include <cathedral/core.hpp>

#include <vector>

namespace cathedral::engine
{
    template <typename T>
    concept has_length = requires(T t) { t.length(); }; // NOLINT

    template <typename T>
    concept has_index_operator = requires(T t) { t[0]; }; // NOLINT

    template <typename T>
    concept is_glm_float_vector_like = has_length<T> && has_index_operator<T> && requires(T t) { // NOLINT
        { t[0] } -> std::convertible_to<float>;
    };

    template <typename T>
    inline constexpr bool is_float = std::is_same_v<T, float>; // NOLINT

    template <typename T>
        requires is_float<T>
    void push_elems(const T& t, std::vector<float>& target)
    {
        target.push_back(static_cast<float>(t));
    }

    template <typename T>
        requires is_glm_float_vector_like<T>
    void push_elems(const T& t, std::vector<float>& target)
    {
        for (int i = 0; i < t.length(); ++i)
        {
            target.push_back(static_cast<float>(t[i]));
        }
    }

    template <typename T>
        requires is_float<T> || is_glm_float_vector_like<T>
    constexpr size_t elem_size()
    {
        if constexpr (is_float<T>)
        {
            return 1;
        }
        else
        {
            return T::length();
        }
    }

    template <typename T0, typename... Ts>
    std::vector<float> pack_vertex_data(const std::vector<T0>& arg, const std::vector<Ts>&... args)
    {
        const size_t vertex_count = arg.size();
        assert(((vertex_count == args.size()) && ...));

        constexpr size_t VERTEX_SIZE = elem_size<T0>() + (elem_size<Ts>() + ...);

        std::vector<float> result;
        result.reserve(VERTEX_SIZE * vertex_count);

        for (size_t i = 0; i < vertex_count; ++i)
        {
            push_elems(arg[i], result);
            (push_elems(args[i], result), ...);
        }

        return result;
    }
} // namespace cathedral::engine