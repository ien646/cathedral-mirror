#pragma once

#include <cinttypes>

namespace cathedral
{
    uint64_t get_uid();

    class uid_type
    {
    public:
        inline uint64_t uid() const { return _uid; }
        
    protected:
        uint64_t _uid = get_uid();
    };
}