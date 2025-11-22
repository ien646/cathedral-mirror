#pragma once

#include <cathedral/core.hpp>

#include <string_view>

namespace cathedral::engine
{
    class node_type
    {
    public:
        using id_type = uint64_t;
        constexpr static auto STR_SIZE = sizeof(id_type);
        using chars_t = std::span<const char, STR_SIZE + 1>;

        constexpr static node_type from_chars(const chars_t chars)
        {
            node_type result{};
            result._id = (static_cast<uint64_t>(chars[0]) << 56)
                         | (static_cast<uint64_t>(chars[1]) << 48)
                         | (static_cast<uint64_t>(chars[2]) << 40)
                         | (static_cast<uint64_t>(chars[3]) << 32)
                         | (static_cast<uint64_t>(chars[4]) << 24)
                         | (static_cast<uint64_t>(chars[5]) << 16)
                         | (static_cast<uint64_t>(chars[6]) << 8)
                         | static_cast<uint64_t>(chars[7]);
            return result;
        }

        constexpr bool operator==(const node_type& rhs) const { return _id == rhs._id; }

        constexpr std::string_view string_view() const
        {
            return std::string_view{ reinterpret_cast<const char*>(&_id), STR_SIZE };
        }

        constexpr void replace(const chars_t sv) { *this = from_chars(sv); }

        constexpr id_type id() const { return _id; }

    private:
        id_type _id;

        constexpr node_type() = default;
    };
} // namespace cathedral::engine