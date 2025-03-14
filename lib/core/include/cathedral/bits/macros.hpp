#pragma once

#ifdef SELF
    #error "SELF macro is somehow already defined"
#endif

#define SELF std::remove_cvref_t<decltype(*this)> // NOLINT

#define CATHEDRAL_NON_COPYABLE(type) /*NOLINT */                                                                            \
    type(const type&) = delete;                                                                                             \
    type& operator=(const type&) = delete

#define CATHEDRAL_DEFAULT_MOVABLE(type) /*NOLINT */                                                                         \
    type(type&&) = default;                                                                                                 \
    type& operator=(type&&) = default
