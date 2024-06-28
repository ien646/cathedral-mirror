#pragma once

#include <cstring>
#include <memory>

namespace cathedral
{
    template <typename T>
    void zero_memory(T* ptr)
    {
        memset(ptr, 0, sizeof(T));
    }

    // Create a struct with all of its data set to zero
    template <typename T>
    T zero_struct()
    {
        T result;
        zero_memory(&result);
        return result;
    }
} // namespace zynr