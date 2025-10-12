#pragma once

namespace cathedral
{
    // A one_time_flag wraps a boolean value that sets itself to false whenever it gets read
    // Useful for "first time" boolean values (first frame, initialized, open once, etc.)
    class one_time_flag
    {
    public:
        explicit one_time_flag(const bool init_value = false)
            : _value(init_value)
        {
        }

        bool get_and_reset()
        {
            const bool result = _value;
            _value = false;
            return result;
        }

        void set(const bool value = true) { _value = value; }

    private:
        bool _value = false;
    };
} // namespace cathedral