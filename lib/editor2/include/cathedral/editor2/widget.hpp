#pragma once

#include <cathedral/editor2/utils.hpp>

namespace cathedral::editor2
{
    struct empty
    {
    };

    template <typename TCallbacks = empty>
    class widget
    {
    public:
        virtual ~widget() = default;
        virtual void tick() = 0;

        TCallbacks& callbacks() { return _callbacks; }

    protected:
        TCallbacks _callbacks;
    };
} // namespace cathedral::editor2