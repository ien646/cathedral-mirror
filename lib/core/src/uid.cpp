#include <cathedral/bits/uid.hpp>

namespace cathedral
{
    uint64_t get_uid()
    {
        static uint64_t accum = 0;
        return accum++;
    }
}