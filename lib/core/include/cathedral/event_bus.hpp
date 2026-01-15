#pragma once

#include <cathedral/core.hpp>
#include <cathedral/ds.hpp>

#include <any>
#include <functional>
#include <memory>
#include <typeindex>
#include <typeinfo>

namespace cathedral
{
    template <typename T>
    concept EventBusEvent = requires { std::is_copy_constructible_v<T>; };

    enum class event_bus_execution_mode : uint8_t
    {
        IMMEDIATE,
        DELAYED
    };

    enum class event_bus_clear_flags : uint8_t
    {
        DELAYED_EVENTS = 1u,
        SUBSCRIPTIONS = 1u << 1,
        ALL = DELAYED_EVENTS | SUBSCRIPTIONS
    };

    CATHEDRAL_ENUM_ENABLE_OR_OPERATOR(event_bus_clear_flags);

    class event_bus
    {
    public:
        using subscription_id_t = uint64_t;
        using callback_t = std::function<void(const std::any&)>;

        struct subscription
        {
            event_bus* bus;
            std::type_index type_index;
            uint64_t subscription_index;

            subscription(event_bus* bus, std::type_index type_index, uint64_t sub_index);
            ~subscription();

            bool is_alive() const;
        };

        template <EventBusEvent T>
        void publish(T&& event, const event_bus_execution_mode mode = event_bus_execution_mode::IMMEDIATE)
        {
            const auto type_index = std::type_index(typeid(T));
            if (mode == event_bus_execution_mode::IMMEDIATE)
            {
                notify_subscribers_immediate(event);
            }
            else
            {
                _delayed_events.emplace(type_index, std::forward<T>(event));
            }
        }

        template <EventBusEvent T>
        std::unique_ptr<subscription> subscribe(callback_t callback)
        {
            const auto type_index = std::type_index(typeid(T));
            const auto sub_index = _subscription_index++;
            _subscriptions[type_index].emplace(sub_index, std::move(callback));

            return std::make_unique<subscription>(this, type_index, sub_index);
        }

        void handle_delayed_events();

        void clear(event_bus_clear_flags clear_flags);

    private:
        unordered_map<std::type_index, std::any> _delayed_events;
        unordered_map<std::type_index, unordered_map<subscription_id_t, callback_t>> _subscriptions;
        uint64_t _subscription_index = 1;

        template <EventBusEvent T>
        void notify_subscribers_immediate(const T& event)
        {
            handle_events(std::type_index(typeid(T)), event);
        }

        void handle_events(std::type_index type, const std::any& event_obj);
    };

    using event_bus_id_t = uint64_t;

    event_bus_id_t register_event_bus(std::string name);
    event_bus_id_t get_event_bus_id(std::string_view);
    event_bus& get_event_bus(event_bus_id_t id);

    using event_bus_subscriptions = std::vector<std::unique_ptr<event_bus::subscription>>;

    class event_bus_subscriber
    {
    protected:
        explicit event_bus_subscriber(event_bus& bus)
            : _event_bus(bus)
        {
        }

        virtual ~event_bus_subscriber() = default;

        template <typename TEvent>
        void subscribe(std::function<void(const TEvent&)> event_handler)
        {
            _subscriptions.push_back(
                _event_bus.subscribe<TEvent>([event_handler = std::move(event_handler)](const std::any& event) {
                    event_handler(std::any_cast<TEvent>(event));
                }));
        }

        event_bus& _event_bus;
        std::vector<std::unique_ptr<event_bus::subscription>> _subscriptions;
    };
} // namespace cathedral