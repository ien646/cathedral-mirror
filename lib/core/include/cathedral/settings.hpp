#pragma once

#include <cathedral/core.hpp>

#include <algorithm>
#include <cstdint>
#include <functional>
#include <memory>
#include <optional>
#include <string>
#include <unordered_map>
#include <variant>
#include <vector>

namespace cathedral
{
    struct setting_enum_value
    {
        std::vector<std::string> enum_values;
        uint32_t current_value = 0;
    };

    enum class setting_type : uint8_t
    {
        EMPTY,
        BOOLEAN,
        INT64,
        DOUBLE,
        STRING,
        ENUM
    };

    class setting_value
    {
    public:
        using variant_t = std::variant<bool, int64_t, double, std::string, setting_enum_value>;

        setting_value() = default;

        explicit(false) setting_value(const bool value)
            : _value(value)
        {
        }

        template <typename T>
            requires(std::is_integral_v<T>)
        explicit(false) setting_value(T v)
            : _value(static_cast<int64_t>(v))
        {
        }

        template <typename T>
            requires(std::is_floating_point_v<T>)
        explicit(false) setting_value(T v)
            : _value(static_cast<double>(v))
        {
        }

        explicit(false) setting_value(std::string v)
            : _value(std::move(v))
        {
        }

        explicit(false) setting_value(setting_enum_value v)
            : _value(std::move(v))
        {
        }

        bool as_bool() const { return std::get<bool>(_value); }

        int64_t as_int() const { return std::get<int64_t>(_value); }

        double as_double() const { return std::get<double>(_value); }

        std::string as_string() const { return std::get<std::string>(_value); }

        setting_enum_value as_enum() const { return std::get<setting_enum_value>(_value); }

        const variant_t& get() const { return _value; }

        void set(variant_t value) { _value = std::move(value); }

        setting_type type() const
        {
            if (std::holds_alternative<bool>(_value))
            {
                return setting_type::BOOLEAN;
            }
            if (std::holds_alternative<int64_t>(_value))
            {
                return setting_type::INT64;
            }
            if (std::holds_alternative<double>(_value))
            {
                return setting_type::DOUBLE;
            }
            if (std::holds_alternative<std::string>(_value))
            {
                return setting_type::STRING;
            }
            if (std::holds_alternative<setting_enum_value>(_value))
            {
                return setting_type::ENUM;
            }
            return setting_type::EMPTY;
        }

    private:
        variant_t _value;
    };

    class settings
    {
    public:
        using subscription_callback_t = std::function<void(const setting_value&)>;

        class subscription
        {
        public:
            explicit subscription(settings& settings, const std::string& key, uint64_t handle)
                : _settings(settings)
                , _key(key)
                , _handle(handle)
            {
            }

            CATHEDRAL_NON_COPYABLE(subscription);

            ~subscription() { _settings.unsubscribe(_key, _handle); }

        private:
            settings& _settings;
            std::string _key;
            uint64_t _handle;
        };

        std::optional<setting_value> get(const std::string& key);
        void set(const std::string& key, const std::optional<setting_value>& value);
        void erase(const std::string& key);

        template <typename T>
        std::optional<T> get(const std::string& key)
        {
            std::optional<setting_value> variant_value = get(key);
            if (!variant_value.has_value())
            {
                return std::nullopt;
            }

            if (!std::holds_alternative<T>(variant_value))
            {
                return std::nullopt;
            }

            return std::get<T>(variant_value);
        }

        template <typename T>
        void set(const std::string& key, const std::optional<T>& value)
        {
            set(key, *value);
        }

        [[nodiscard]] std::unique_ptr<subscription> subscribe(const std::string& key, subscription_callback_t call)
        {
            uint64_t handle = _handle_counter++;
            _subscriptions[key].push_back(std::pair{ handle, std::move(call) });
            return std::make_unique<subscription>(*this, key, handle);
        }

        const std::unordered_map<std::string, setting_value>& all_entries() const;

    private:
        uint64_t _handle_counter = 0;
        std::unordered_map<std::string, setting_value> _entries;
        std::unordered_map<std::string, std::vector<std::pair<uint64_t, subscription_callback_t>>> _subscriptions;

        void unsubscribe(const std::string& key, uint64_t handle)
        {
            auto& key_calls = _subscriptions[key];
            const auto it = std::ranges::find_if(key_calls, [handle](const auto& v) { return v.first == handle; });
            if (it != key_calls.end())
            {
                key_calls.erase(it);
            }
            else
            {
                log_error(std::format("Attempt to unsubscribe non existing handle '{}' for setting id '{}'", handle, key));
            }
        }
    };
} // namespace cathedral