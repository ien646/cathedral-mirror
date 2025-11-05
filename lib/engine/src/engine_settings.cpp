#include <cathedral/engine/engine_settings.hpp>

namespace cathedral
{
    using enum engine::engine_setting;

    // clang-format off
    const unordered_map<engine::engine_setting, setting_value> default_settings = {
        { UPLOAD_QUEUE_SIZE_MB,128LL },
        { VSYNC_ENABLED, true },
        { VSYNC_MAILBOX, true },
        { MSAA_SAMPLES, setting_enum_value{.enum_values = {"1", "2", "4", "8"}, .current_value = 0, } },
        { MSAA_SAMPLE_SHADING, true }
    };
    // clang-format on

    template <>
    setting_type get_setting_type<engine::engine_setting>(const engine::engine_setting e)
    {
        switch (e)
        {
        case UPLOAD_QUEUE_SIZE_MB:
            return setting_type::INT64;
        case VSYNC_ENABLED:
            return setting_type::BOOLEAN;
        case VSYNC_MAILBOX:
            return setting_type::BOOLEAN;
        case MSAA_SAMPLES:
            return setting_type::ENUM;
        case MSAA_SAMPLE_SHADING:
            return setting_type::BOOLEAN;
        default:
            CRITICAL_ERROR(std::format("Unhandled engine setting: {}", magic_enum::enum_name(e)));
        }
    }

    template <>
    std::optional<setting_value> get_default_value<engine::engine_setting>(const engine::engine_setting e)
    {
        if (default_settings.contains(e))
        {
            return default_settings.at(e);
        }
        log_warning(std::format("Attempt to get undefined default engine setting '{}'", magic_enum::enum_name(e)));
        return {};
    }
} // namespace cathedral