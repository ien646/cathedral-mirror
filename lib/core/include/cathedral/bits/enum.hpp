#pragma once

#include <magic_enum.hpp>

#define CATHEDRAL_ENUM_TO_CSTR(e) std::string{ magic_enum::enum_name(e) }.c_str()

#define CATHEDRAL_ENUM_ENABLE_OR_OPERATOR(enum_type)                                                                        \
    inline enum_type operator|(enum_type lhs, enum_type rhs)                                                                \
    {                                                                                                                       \
        using itype = std::underlying_type_t<enum_type>;                                                                    \
        return static_cast<enum_type>(static_cast<itype>(lhs) | static_cast<itype>(rhs));                                   \
    }

#define CATHEDRAL_ENUM_ENABLE_AND_OPERATOR(enum_type)                                                                       \
    inline enum_type operator&(enum_type lhs, enum_type rhs)                                                                \
    {                                                                                                                       \
        using itype = std::underlying_type_t<enum_type>;                                                                    \
        return static_cast<enum_type>(static_cast<itype>(lhs) & static_cast<itype>(rhs));                                   \
    }

#define CATHEDRAL_ENUM_ENABLE_XOR_OPERATOR(enum_type)                                                                       \
    inline enum_type operator^(enum_type lhs, enum_type rhs)                                                                \
    {                                                                                                                       \
        using itype = std::underlying_type_t<enum_type>;                                                                    \
        return static_cast<enum_type>(static_cast<itype>(lhs) ^ static_cast<itype>(rhs));                                   \
    }