#include <cathedral/event_bus.hpp>

#include <ranges>

namespace cathedral
{
    event_bus::subscription::subscription(event_bus* bus, const std::type_index type_index, const uint64_t sub_index)
        : bus(bus)
        , type_index(type_index)
        , subscription_index(sub_index)
    {
    }

    event_bus::subscription::~subscription()
    {
        bus->_subscriptions[type_index].erase(subscription_index);
    }

    bool event_bus::subscription::is_alive() const
    {
        return bus->_subscriptions.contains(type_index) && bus->_subscriptions[type_index].contains(subscription_index);
    }

    void event_bus::handle_delayed_events()
    {
        for (const auto& [type, event] : _delayed_events)
        {
            handle_events(type, event);
        }
        _delayed_events.clear();
    }

    void event_bus::clear(const event_bus_clear_flags clear_flags)
    {
        if ((clear_flags | event_bus_clear_flags::DELAYED_EVENTS) == clear_flags)
        {
            _delayed_events.clear();
        }
        if ((clear_flags | event_bus_clear_flags::SUBSCRIPTIONS) == clear_flags)
        {
            _subscriptions.clear();
        }
    }

    void event_bus::handle_events(const std::type_index type, const std::any& event_obj)
    {
        size_t handled_count = 0;
        for (const auto& callback : _subscriptions[type] | std::views::values)
        {
            callback(event_obj);
            ++handled_count;
        }

        if (handled_count == 0)
        {
            log_warning(std::format("Event with type name '{}' was handled but nobody listened", type.name()));
        }
    }

    namespace
    {
        std::vector<event_bus> event_buses;
        std::vector<std::string> event_bus_names;
    } // namespace

    event_bus_id_t register_event_bus(std::string name)
    {
        if (const auto it = std::ranges::find(event_bus_names, name); it != event_bus_names.end())
        {
            log_warning(std::format("Event bus with name '{}' was already registered", name));
            return std::distance(event_bus_names.begin(), it);
        }

        event_bus_names.push_back(std::move(name));
        event_buses.emplace_back();

        return event_buses.size() - 1;
    }

    event_bus_id_t get_event_bus_id(std::string_view name)
    {
        const auto it = std::ranges::find(event_bus_names, name);
        if (it == event_bus_names.end())
        {
            CRITICAL_ERROR(std::format("Unable to find event bus with name '{}'", name));
        }
        return std::distance(event_bus_names.begin(), it);
    }

    event_bus& get_event_bus(const event_bus_id_t id)
    {
        CRITICAL_CHECK(event_buses.size() > id, std::format("Unable to obtain event bus for id '{}'", id));
        return event_buses[id];
    }
} // namespace cathedral