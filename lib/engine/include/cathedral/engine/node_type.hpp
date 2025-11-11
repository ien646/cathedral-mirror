#pragma once

#include <cathedral/core.hpp>

#include <string_view>

namespace cathedral::engine
{
    class node_type
    {
    public:
        using id_type = uint64_t;

        constexpr static node_type from_chars(const std::string_view sv)
        {
            CRITICAL_CHECK(sv.size() == 8, "node type id requires 8 chars exactly");
            node_type result{};
            result._id = *reinterpret_cast<const id_type*>(sv.data());
            return result;
        }

        bool operator==(const node_type& rhs) const { return _id == rhs._id; }

        constexpr std::string_view string_view() const { return std::string_view{ reinterpret_cast<const char*>(&_id), 8 }; }

        void replace(const std::string_view sv) { *this = from_chars(sv); }

        id_type id() const { return _id; }

        friend struct std::hash<node_type>;

    private:
        id_type _id;

        constexpr node_type() = default;
    };
} // namespace cathedral::engine