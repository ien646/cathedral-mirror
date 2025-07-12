#pragma once

namespace cathedral::editor2
{
    template <typename T>
    void callback(T& call)
    {
        if (call != nullptr)
        {
            call();
        }
    }
} // namespace cathedral::editor2