#pragma once

#include <cathedral/settings.hpp>
#include <memory>

namespace cathedral::engine
{
    enum class engine_setting : uint8_t
    {
        UPLOAD_QUEUE_SIZE_MB,
        VSYNC_ENABLED,
        VSYNC_MAILBOX,
        MSAA_SAMPLES
    };

    class engine_settings_interface
    {
    public:
        explicit engine_settings_interface(std::shared_ptr<settings> settings);

        setting_value get(engine_setting key) const;
        void set(engine_setting key, setting_value value) const;
        void erase(engine_setting key) const;

        std::string get_setting_key(engine_setting setting) const;

        [[nodiscard]] std::unique_ptr<settings::subscription> subscribe(
            engine_setting setting,
            std::function<void(const setting_value&)> call) const;

    private:
        std::shared_ptr<settings> _settings;
    };
} // namespace cathedral::engine