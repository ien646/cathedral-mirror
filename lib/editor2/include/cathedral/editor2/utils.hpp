#pragma once

namespace cathedral::editor2
{
    template <typename T, typename ... TArgs>
    void callback(T& call, TArgs... args)
    {
        if (call != nullptr)
        {
            call(args...);
        }
    }
} // namespace cathedral::editor2