#pragma once

#include <optional>
#include <string>
#include <unordered_map>
#include <variant>
#include <vector>

namespace cathedral
{
    class setting_value
    {
    public:
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

    private:
        std::variant<int64_t, double, std::string> _value;
    };

    class settings
    {
    public:
        static std::optional<setting_value> get(const std::string& key);
        static void set(const std::string& key, const std::optional<setting_value>& value);
        static void erase(const std::string& key);

        template <typename T>
        static std::optional<T> get(const std::string& key)
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
        static void set(const std::string& key, const std::optional<T>& value)
        {
            if (!value.has_value())
            {
                erase(key);
                return;
            }

            set(key, *value);
        }

        static const std::unordered_map<std::string, setting_value>& all_entries();
    };
} // namespace cathedral