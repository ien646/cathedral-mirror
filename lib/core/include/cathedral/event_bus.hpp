#pragma once

#include <cathedral/core.hpp>
#include <cathedral/ds.hpp>

#include <boost/preprocessor.hpp>

#include <any>
#include <functional>
#include <memory>
#include <typeindex>
#include <typeinfo>

namespace cathedral
{
    struct event
    {
        uint64_t id;
        event();
    };

    template <typename T>
    constexpr void validate_event_type()
    {
        static_assert(std::is_base_of_v<event, T>, "Event bus event objects must inherit 'cathedral::event'");
        static_assert(std::is_copy_constructible_v<T>, "Event bus event objects must be copy constructible");
    }

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

        template <typename T>
        void publish(T&& event, const event_bus_execution_mode mode = event_bus_execution_mode::IMMEDIATE)
        {
            validate_event_type<T>();
            const auto type_index = std::type_index(typeid(T));
            if (mode == event_bus_execution_mode::IMMEDIATE)
            {
                notify_subscribers_immediate(event);
            }
            else
            {
                _delayed_events.emplace(type_index, std::forward<T>(event));
            }

            if constexpr (is_debug_build())
            {
                log_info(
                    std::format(
                        "Event published: [id: {}, type: {}, mode: {}]",
                        static_cast<cathedral::event>(event).id,
                        std::type_index(typeid(T)).name(),
                        mode == event_bus_execution_mode::IMMEDIATE ? "Immediate" : "Delayed"));
            }
        }

        template <typename T>
            requires(std::is_default_constructible_v<T>)
        void publish(const event_bus_execution_mode mode = event_bus_execution_mode::IMMEDIATE)
        {
            validate_event_type<T>();
            publish(T{}, mode);
        }

        template <typename T>
        std::unique_ptr<subscription> subscribe(callback_t callback)
        {
            validate_event_type<T>();

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

        template <typename T>
        void notify_subscribers_immediate(const T& event)
        {
            validate_event_type<T>();
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
            validate_event_type<TEvent>();

            _subscriptions.push_back(
                _event_bus.subscribe<TEvent>([event_handler = std::move(event_handler)](const std::any& event) {
                    event_handler(std::any_cast<TEvent>(event));
                }));
        }

        event_bus& _event_bus;
        std::vector<std::unique_ptr<event_bus::subscription>> _subscriptions;
    };
} // namespace cathedral

#define CATHEDRAL_DECLARE_EVENTS_MACRO_(r, body, elem)                                                                      \
    struct elem : cathedral::event                                                                                          \
    {                                                                                                                       \
        body                                                                                                                \
    };

#define CATHEDRAL_DECLARE_EVENTS(struct_body, ...)                                                                          \
    BOOST_PP_SEQ_FOR_EACH(CATHEDRAL_DECLARE_EVENTS_MACRO_, struct_body, BOOST_PP_VARIADIC_TO_SEQ(__VA_ARGS__))