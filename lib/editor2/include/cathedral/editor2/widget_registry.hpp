#pragma once

#include <cathedral/editor2/widget.hpp>

#include <memory>

namespace cathedral::editor2
{
    class widget_registry
    {
    public:
        void add_widget(const std::shared_ptr<widget_base>& widget);
        void remove_widget(const widget_base& widget);
        void remove_widget(const std::shared_ptr<widget_base>& widget);

        void tick() const;

    private:
        std::vector<std::shared_ptr<widget_base>> _widgets;
    };
} // namespace cathedral::editor2