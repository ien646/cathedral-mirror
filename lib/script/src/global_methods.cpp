#include <cathedral/script/global_methods.hpp>

#include "cathedral/bits/log.hpp"

#include <cathedral/script/global_state.hpp>

namespace cathedral::script
{
    void register_global_methods(global_state& s)
    {
        auto ns = luabridge::getGlobalNamespace(*s);
        ns.addFunction("log_info", [](const std::string& msg) { log_info(msg); });
        ns.addFunction("log_warning", [](const std::string& msg) { log_warning(msg); });
        ns.addFunction("log_error", [](const std::string& msg) { log_error(msg); });
    }
} // namespace cathedral::script