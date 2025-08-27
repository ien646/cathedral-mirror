#pragma once
#include "cathedral/settings.hpp"

namespace cathedral::engine
{
    enum class engine_setting
    {
        UPLOAD_QUEUE_SIZE_MB,
    };

    class engine_settings_interface
    {
    public:
        static setting_value get(engine_setting key);
        static void set(engine_setting key, setting_value value);
        static void erase(engine_setting key);
    };
}