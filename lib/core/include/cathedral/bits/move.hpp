#pragma once

#include <cathedral/bits/inline.hpp>

#include <type_traits>

namespace cathedral
{
    template <typename T>
    [[nodiscard]] CATHEDRAL_FORCE_INLINE constexpr std::remove_reference_t<T>&& MOVE(T&& t) noexcept // NOLINT
    {
        // Basically std::move, but statically asserts that it's never called on const types or rvalues, in order to avoid
        // subtle bugs and unnecesary move calls when refactoring code
        static_assert(std::is_lvalue_reference_v<T>, "Calling move on an rvalue has no effect");
        static_assert(
            !std::is_same_v<std::remove_reference_t<T>&, const std::remove_reference_t<T>&>,
            "Calling move on a const type has no effect");

        return static_cast<std::remove_reference_t<T>&&>(t);
    }
} // namespace cathedral