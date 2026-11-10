#pragma once

// clang-format off
extern "C"
{
#include <lauxlib.h>
}
#include <LuaBridge/LuaBridge.h>
// clang-format on

namespace cathedral::script
{
    class global_state
    {
    public:
        global_state();
        lua_State* get() const;

        void set_string(const std::string& name, const std::string& value) const;
        void collect_garbage() const;

        lua_State* operator*() const;

        luabridge::LuaRef create_environment() const;

    private:
        lua_State* _lua_state = nullptr;
    };

    global_state& get_global_state();
} // namespace cathedral::script