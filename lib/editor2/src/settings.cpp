#include <cathedral/editor2/settings.hpp>

namespace cathedral
{
    using enum editor2::editor_setting;
    using enum setting_type;

    namespace
    {
        unordered_map<editor2::editor_setting, setting_value> default_values = { { TEXT_SCALE, 1.0 },
                                                                                 { EDITOR_WINDOW_SETUP_COMPLETE, false },
                                                                                 { FONT_MANAGER_SETUP_COMPLETE, false },
                                                                                 { MATERIAL_MANAGER_SETUP_COMPLETE, false },
                                                                                 { MESH_MANAGER_SETUP_COMPLETE, false },
                                                                                 { SCRIPT_MANAGER_SETUP_COMPLETE, false } };
    }

    template <>
    setting_type get_setting_type<editor2::editor_setting>(editor2::editor_setting e)
    {
        switch (e)
        {
        case TEXT_SCALE:
            return DOUBLE;
        case EDITOR_WINDOW_SETUP_COMPLETE:
        case FONT_MANAGER_SETUP_COMPLETE:
        case MATERIAL_MANAGER_SETUP_COMPLETE:
        case MESH_MANAGER_SETUP_COMPLETE:
        case SCRIPT_MANAGER_SETUP_COMPLETE:
            return BOOLEAN;
        default:
            CRITICAL_ERROR(std::format("Undedined type for setting '{}'", magic_enum::enum_name(e)));
        }
    }

    template <>
    std::optional<setting_value> get_default_value<editor2::editor_setting>(editor2::editor_setting e)
    {
        CRITICAL_CHECK(
            default_values.contains(e),
            std::format("Undefined default value for setting '{}'", magic_enum::enum_name(e)));
        return default_values.at(e);
    }
} // namespace cathedral