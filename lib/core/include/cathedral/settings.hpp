#pragma once

#include <cstdint>
#include <optional>
#include <string>
#include <unordered_map>
#include <variant>
#include <vector>

namespace cathedral
{
    enum class setting_type : uint8_t
    {
        EMPTY,
        INT64,
        DOUBLE,
        STRING
    };

    class setting_value
    {
    public:
        using variant_t = std::variant<int64_t, double, std::string>;

        setting_value() = default;

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

        int64_t as_int() const { return std::get<int64_t>(_value); }

        double as_double() const { return std::get<double>(_value); }

        std::string as_string() const { return std::get<std::string>(_value); }

        const variant_t& get() const { return _value; }

        void set(variant_t value) { _value = std::move(value); }

        setting_type type() const
        {
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
            return setting_type::EMPTY;
        }

    private:
        variant_t _value;
    };

    class settings
    {
    public:
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
            if (!value.has_value())
            {
                erase(key);
                return;
            }

            set(key, *value);
        }

        const std::unordered_map<std::string, setting_value>& all_entries() const;

    private:
        std::unordered_map<std::string, setting_value> _entries;
    };
} // namespace cathedral