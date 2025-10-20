#pragma once

#include <cathedral/settings_interface.hpp>

namespace cathedral::editor2
{
    enum class editor_settings
    {
        TEXT_SCALE,
        FONT_MANAGER_SETUP_COMPLETE,
        MATERIAL_MANAGER_SETUP_COMPLETE,
        EDITOR_WINDOW_SETUP_COMPLETE
    };
} // namespace cathedral::editor2

namespace cathedral
{
    template <>
    setting_type get_setting_type(editor2::editor_settings e);

    template <>
    std::optional<setting_value> get_default_value(editor2::editor_settings e);
} // namespace cathedral

namespace cathedral::editor2
{
    using editor_settings_interface = settings_interface<editor_settings>;
}