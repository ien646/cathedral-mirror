#pragma once

#include <cathedral/core.hpp>
#include <cathedral/editor2/utils.hpp>

FORWARD_CLASS(cathedral::editor2, widget_registry);

namespace cathedral::editor2
{
    struct empty
    {
    };

    class widget_base
    {
    public:
        explicit widget_base();
        CATHEDRAL_NON_COPYABLE(widget_base);
        CATHEDRAL_DEFAULT_MOVABLE(widget_base);
        virtual ~widget_base() = default;

        virtual void tick() = 0;
        uint64_t id() const;

        bool operator==(const widget_base& other) const;

    protected:
        uint64_t _id;
    };

    template <typename TCallbacks = empty>
    class widget : public widget_base
    {
    public:
        using widget_base::widget_base;

        TCallbacks& callbacks() { return _callbacks; }

    protected:
        TCallbacks _callbacks;
    };
} // namespace cathedral::editor2