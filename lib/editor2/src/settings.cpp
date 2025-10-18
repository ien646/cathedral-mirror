#include <cathedral/editor2/settings.hpp>

namespace cathedral
{
    using enum editor2::editor_settings;
    using enum setting_type;

    namespace
    {
        std::unordered_map<editor2::editor_settings, setting_value> default_values = { { TEXT_SCALE, 1.0 },
                                                                                       { EDITOR_WINDOW_SETUP_COMPLETE,
                                                                                         false } };
    }

    template <>
    setting_type get_setting_type<editor2::editor_settings>(editor2::editor_settings e)
    {
        switch (e)
        {
        case TEXT_SCALE:
            return DOUBLE;
        case EDITOR_WINDOW_SETUP_COMPLETE:
            return BOOLEAN;
        default:
            CRITICAL_ERROR(std::format("Undedined type for setting '{}'", magic_enum::enum_name(e)));
        }
    }

    template <>
    std::optional<setting_value> get_default_value<editor2::editor_settings>(editor2::editor_settings e)
    {
        CRITICAL_CHECK(
            default_values.contains(e),
            std::format("Undefined default value for setting '{}'", magic_enum::enum_name(e)));
        return default_values.at(e);
    }
} // namespace cathedral