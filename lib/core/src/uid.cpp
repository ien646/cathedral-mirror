#include <cathedral/bits/uid.hpp>

#include <atomic>

namespace cathedral
{
    std::atomic<uint64_t> _current_uid = 0;

    uint64_t get_uid()
    {
        return _current_uid++;
    }

    uid_type::uid_type()
        : _uid(get_uid())
    {
    }

    bool uid_type::operator==(const uid_type& rhs) const
    {
        return _uid == rhs.uid();
    }

    bool uid_type::operator!=(const uid_type& rhs) const
    {
        return _uid != rhs.uid();
    }
} // namespace cathedral