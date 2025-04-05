#pragma once

#include <type_traits>

namespace cathedral
{
    template <typename T>
    struct inout_param
    {
        static_assert(!std::is_const_v<T>, "inout_param cannot bind to const type");

        T& ref;

        explicit inout_param(T& ref)
            : ref(ref)
        {
        }

        T& operator*() { return ref; }

        const T& operator*() const { return ref; }

        T* operator->() { return &ref; }

        const T* operator->() const { return &ref; }
    };

    template <typename T>
    struct out_param
    {
        static_assert(!std::is_const_v<T>, "out_param cannot bind to const type");

        T& ref;

        explicit out_param(T& ref)
            : ref(ref)
        {
        }

        T& operator*() { return ref; }

        const T& operator*() const { return ref; }

        T* operator->() { return &ref; }

        const T* operator->() const { return &ref; }
    };
} // namespace cathedral