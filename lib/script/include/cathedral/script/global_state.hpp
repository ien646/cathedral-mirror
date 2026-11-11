#pragma once

// clang-format off
extern "C"
{
#include <lauxlib.h>
}
#include <LuaBridge/LuaBridge.h>
// clang-format on

// Inform LuaBridge that automatic C++ <-> Lua enum conversions are desired
namespace luabridge
{
    template <typename T>
        requires std::is_enum_v<T>
    struct Stack<T> : Enum<T>
    {
    };
}

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