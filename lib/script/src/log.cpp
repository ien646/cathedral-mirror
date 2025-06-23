#include <cathedral/script/log.hpp>

#include <cathedral/core.hpp>

namespace cathedral::script
{
    void log_initializer::initialize(state& s)
    {
        s.set_function("log_error", &log_error);
        s.set_function("log_warning", &log_warning);
        s.set_function("log_info", &log_info);
    }
} // namespace cathedral::script