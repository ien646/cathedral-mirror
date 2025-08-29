#include <cathedral/engine/engine_settings.hpp>

#include <cathedral/bits/error.hpp>

#include <magic_enum.hpp>

namespace cathedral::engine
{
    namespace
    {
        const std::unordered_map<engine_setting, setting_value> default_settings = { { engine_setting::UPLOAD_QUEUE_SIZE_MB,
                                                                                       128 } };

        std::string enum2key(const engine_setting setting)
        {
            constexpr auto ENGINE_PREFIX = "cathedral::engine::";
            return ENGINE_PREFIX + std::string{ magic_enum::enum_name(setting) };
        }
    } // namespace

    engine_settings_interface::engine_settings_interface(std::shared_ptr<settings> settings)
        : _settings(std::move(settings))
    {
    }

    setting_value engine_settings_interface::get(const engine_setting key) const
    {
        const auto str_key = enum2key(key);
        auto val = _settings->get(str_key);
        if (val.has_value())
        {
            return *val;
        }

        if (default_settings.contains(key))
        {
            return default_settings.at(key);
        }

        CRITICAL_ERROR(std::format("Unhandled engine setting: {}", str_key));
    }

    void engine_settings_interface::set(const engine_setting key, setting_value value) const
    {
        _settings->set(enum2key(key), value);
    }

    void engine_settings_interface::erase(const engine_setting key) const
    {
        _settings->erase(enum2key(key));
    }

    std::string engine_settings_interface::get_setting_key(const engine_setting setting) const
    {
        return enum2key(setting);
    }
} // namespace cathedral::engine