#pragma once

#include <cathedral/settings.hpp>
#include <memory>

#include <magic_enum.hpp>

namespace cathedral
{
    template <typename TSettingsEnum>
        requires(std::is_enum_v<TSettingsEnum>)
    setting_type get_setting_type(TSettingsEnum e) = delete;

    template <typename TSettingsEnum>
        requires(std::is_enum_v<TSettingsEnum>)
    std::optional<setting_value> get_default_value([[maybe_unused]] TSettingsEnum e) = delete;

    template <typename TSettingsEnum>
        requires(std::is_enum_v<TSettingsEnum>)
    class settings_interface
    {
    public:
        explicit settings_interface(std::shared_ptr<settings> settings, std::string prefix)
            : _settings(std::move(settings))
            , _prefix(std::move(prefix))
        {
            auto check = [this](const TSettingsEnum setting, const setting_type type) {
                const auto& value = get(setting);
                if (value.type() != type)
                {
                    log_error(
                        std::format(
                            "Invalid setting type '{}' for key '{}'. Expected type '{}'. Replacing with default."
                            "value.",
                            magic_enum::enum_name(value.type()),
                            magic_enum::enum_name(setting),
                            magic_enum::enum_name(type)));
                    auto default_value = get_default_value(setting);
                    if (default_value)
                    {
                        set(setting, std::move(*default_value));
                    }
                }
            };

            for (const auto& value : magic_enum::enum_values<TSettingsEnum>())
            {
                check(value, get_setting_type(value));
            }
        }

        setting_value get(TSettingsEnum key) const
        {
            const auto str_key = _prefix + std::string{ std::string{ magic_enum::enum_name(key) } };
            const auto val = _settings->get(str_key);

            if (!val.has_value())
            {
                auto value = get_default_value(key);

                if (!value.has_value())
                {
                    CRITICAL_ERROR(std::format("Unhandled setting key '{}'", static_cast<int>(key)));
                }

                set(key, *value);
                return *value;
            }

            if (val->type() != get_setting_type(key))
            {
                log_error(
                    std::format(
                        "Setting '{}', has invalid type '{}' (expected type '{}'). Resetting to default value.",
                        magic_enum::enum_name(key),
                        magic_enum::enum_name(val->type()),
                        magic_enum::enum_name(get_setting_type(key))));
                auto value = get_default_value(key);

                if (!value.has_value())
                {
                    CRITICAL_ERROR(std::format("Unhandled setting key '{}'", static_cast<int>(key)));
                }

                set(key, *value);
                return *value;
            }

            return *val;
        }

        void set(const TSettingsEnum key, setting_value value) const
        {
            const auto str_key = _prefix + std::string{ magic_enum::enum_name(key) };
            _settings->set(str_key, std::move(value));
        }

        void erase(const TSettingsEnum key) const
        {
            const auto str_key = _prefix + std::string{ magic_enum::enum_name(key) };
            _settings->erase(str_key);
        }

        std::string get_setting_key(const TSettingsEnum setting) const
        {
            return _prefix + std::string{ magic_enum::enum_name(setting) };
        }

        [[nodiscard]] std::unique_ptr<settings::subscription> subscribe(
            TSettingsEnum setting,
            std::function<void(const setting_value&)> call) const
        {
            const auto str_key = _prefix + std::string{ magic_enum::enum_name(setting) };
            return _settings->subscribe(str_key, std::move(call));
        }

    private:
        std::shared_ptr<settings> _settings;
        std::string _prefix;
    };
} // namespace cathedral