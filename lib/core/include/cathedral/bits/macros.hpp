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

#define CATHEDRAL_CONCAT(a, b) a##b
#define CATHEDRAL_UNIQUE_SUFFIX(x, id) CATHEDRAL_CONCAT(x, id)

#ifndef CATHEDRAL_PADDING_BYTE
    #define CATHEDRAL_PADDING_BYTE 0xADull
#endif

#define CATHEDRAL_PADDING_8 const uint8_t CATHEDRAL_UNIQUE_SUFFIX(_cathedral_padding_, __COUNTER__) = CATHEDRAL_PADDING_BYTE

#define CATHEDRAL_PADDING_16                                                                                                \
    const uint16_t CATHEDRAL_UNIQUE_SUFFIX(_cathedral_padding_, __COUNTER__) = CATHEDRAL_PADDING_BYTE |                     \
                                                                               (CATHEDRAL_PADDING_BYTE << 8)
#define CATHEDRAL_PADDING_32                                                                                                \
    const uint32_t CATHEDRAL_UNIQUE_SUFFIX(_cathedral_padding_, __COUNTER__) =                                              \
        CATHEDRAL_PADDING_BYTE | (CATHEDRAL_PADDING_BYTE << 8) | (CATHEDRAL_PADDING_BYTE << 16) |                           \
        (CATHEDRAL_PADDING_BYTE << 24)

#define CATHEDRAL_PADDING_64                                                                                                \
    const uint64_t CATHEDRAL_UNIQUE_SUFFIX(_cathedral_padding_, __COUNTER__) =                                              \
        CATHEDRAL_PADDING_BYTE | (CATHEDRAL_PADDING_BYTE << 8) | (CATHEDRAL_PADDING_BYTE << 16) |                           \
        (CATHEDRAL_PADDING_BYTE << 24) | (CATHEDRAL_PADDING_BYTE << 32) | (CATHEDRAL_PADDING_BYTE << 40) |                  \
        (CATHEDRAL_PADDING_BYTE << 48) | (CATHEDRAL_PADDING_BYTE << 56)
