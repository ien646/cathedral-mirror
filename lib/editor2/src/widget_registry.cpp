#include "ien/algorithm.hpp"

#include <cathedral/editor2/widget_registry.hpp>

namespace cathedral::editor2
{
    void widget_registry::add_widget(const std::shared_ptr<widget_base>& widget)
    {
        const auto it = std::ranges::find(_widgets, widget);
        if (it == _widgets.end())
        {
            _widgets.push_back(widget);
        }
    }

    void widget_registry::remove_widget(const widget_base& widget)
    {
        auto it = std::ranges::find_if(_widgets, [&widget](const std::shared_ptr<widget_base>& other) {
            return *other == widget;
        });

        if (it != _widgets.end())
        {
            ien::erase_unsorted(_widgets, it);
        }
    }

    void widget_registry::remove_widget(const std::shared_ptr<widget_base>& widget)
    {
        const auto it = std::ranges::find(_widgets, widget);
        if (it != _widgets.end())
        {
            ien::erase_unsorted(_widgets, it);
        }
    }

    void widget_registry::tick() const
    {
        for (const auto& widget : _widgets)
        {
            widget->tick();
        }
    }
} // namespace cathedral::editor2