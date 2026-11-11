#include <cathedral/script/global_state.hpp>

#include <cathedral/core.hpp>
#include <cathedral/script/engine/input.hpp>
#include <cathedral/script/engine/scene.hpp>
#include <cathedral/script/global_methods.hpp>
#include <cathedral/script/types/nodes.hpp>
#include <cathedral/script/types/vec.hpp>

extern "C"
{
#include <lualib.h>
}

namespace cathedral::script
{
    global_state::global_state()
    {
        _lua_state = luaL_newstate();
        CRITICAL_CHECK(_lua_state != nullptr, "Unable to initialize Lua state");

        luaL_openlibs(_lua_state);

        register_vec_types(*this);
        register_global_methods(*this);
        register_scene(*this);
        register_nodes(*this);
        register_input(*this);
    }

    lua_State* global_state::get() const
    {
        return _lua_state;
    }

    void global_state::set_string(const std::string& name, const std::string& value) const
    {
        luabridge::getGlobalNamespace(_lua_state).addVariable(name.c_str(), value.c_str());
    }

    void global_state::collect_garbage() const
    {
        lua_gc(_lua_state, LUA_GCCOLLECT, 0);
    }

    lua_State* global_state::operator*() const
    {
        return _lua_state;
    }

    luabridge::LuaRef global_state::create_environment() const
    {
        lua_newtable(_lua_state);                               // => ENV
        lua_newtable(_lua_state);                               // => METATABLE
        lua_pushglobaltable(_lua_state);                        // => GLOBAL
        lua_setfield(_lua_state, -2, "__index");                // <= GLOBAL
        lua_setmetatable(_lua_state, -2);                       // <= METATABLE
        auto result = luabridge::LuaRef::fromStack(_lua_state); // ...// <= ENV

        return result;
    }

    global_state& get_global_state()
    {
        static global_state state;
        return state;
    }
} // namespace cathedral::script