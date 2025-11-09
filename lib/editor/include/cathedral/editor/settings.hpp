#pragma once

#include <cathedral/settings_interface.hpp>

namespace cathedral::editor
{
    enum class editor_setting
    {
        TEXT_SCALE,
        EDITOR_WINDOW_SETUP_COMPLETE,
        FONT_MANAGER_SETUP_COMPLETE,
        MATERIAL_MANAGER_SETUP_COMPLETE,
        MESH_MANAGER_SETUP_COMPLETE,
        SCRIPT_MANAGER_SETUP_COMPLETE
    };
} // namespace cathedral::editor

namespace cathedral
{
    template <>
    setting_type get_setting_type(editor::editor_setting e);

    template <>
    std::optional<setting_value> get_default_value(editor::editor_setting e);
} // namespace cathedral

namespace cathedral::editor
{
    using editor_settings_interface = settings_interface<editor_setting>;
}