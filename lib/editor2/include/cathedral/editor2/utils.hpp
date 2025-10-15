#pragma once

namespace cathedral::editor2
{
    template <typename T, typename... TArgs>
    void try_call(T& c, TArgs&... args)
    {
        if (c)
        {
            c(args...);
        }
    }
} // namespace cathedral::editor2