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

        inout_param& operator=(const T& ref)
        {
            this->ref = ref;
            return *this;
        }
    };

    template <typename T>
    using out_param = T&;
} // namespace cathedral