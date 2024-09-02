#pragma once

#include <cinttypes> // IWYU pragma: keep
#include <memory> // IWYU pragma: keep

namespace cathedral
{
    uint64_t get_uid();

    class uid_type
    {
    public:
        uid_type();

        inline uint64_t uid() const { return _uid; }

        bool operator==(const uid_type& rhs) const;

    protected:
        const uint64_t _uid;
    };
} // namespace cathedral

template <>
struct std::hash<cathedral::uid_type>
{
    size_t operator()(const cathedral::uid_type& k) const { return k.uid(); }
};