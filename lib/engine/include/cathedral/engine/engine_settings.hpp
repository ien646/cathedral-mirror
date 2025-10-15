#pragma once

#include <cathedral/settings.hpp>
#include <cathedral/settings_interface.hpp>

#include <memory>

namespace cathedral::engine
{
    enum class engine_setting : uint8_t
    {
        UPLOAD_QUEUE_SIZE_MB,
        VSYNC_ENABLED,
        VSYNC_MAILBOX,
        MSAA_SAMPLES,
        MSAA_SAMPLE_SHADING
    };
} // namespace cathedral::engine

namespace cathedral
{
    template <>
    setting_type get_setting_type<engine::engine_setting>(engine::engine_setting e);

    template <>
    std::optional<setting_value> get_default_value<engine::engine_setting>(engine::engine_setting e);
} // namespace cathedral

namespace cathedral::engine
{
    using engine_settings_interface = settings_interface<engine_setting>;
}