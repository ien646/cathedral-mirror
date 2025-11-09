#include <cathedral/editor/callback_decl.hpp>

#include <type_traits>
#include <utility>

#ifndef CATHEDRAL_CALLBACK_IMPLEMENTATION_INCLUDED
    #define CATHEDRAL_CALLBACK_IMPLEMENTATION_INCLUDED
#else
    #error "Do not include 'callback_impl.hpp' in headers"
#endif

namespace cathedral::internal
{
    template <typename T>
    constexpr void check_callback()
    {
        static_assert(
            std::is_void_v<T>,
            "\n------------------------------------------------------\nCALLBACK() requires a full call expression: "
            "\n\tCALLBACK(accepted()) -> "
            "OK\n\tCALLBACK(accepted))  -> ERROR\n------------------------------------------------------");
    }
} // namespace cathedral::internal

#define CALLBACK(cb)                                                                                                        \
    cathedral::internal::check_callback<decltype(CATHEDRAL_CALLBACKS_STRUCT_NAME.cb)>();                                    \
    static_cast<void>(CATHEDRAL_CALLBACKS_STRUCT_NAME.cb)
