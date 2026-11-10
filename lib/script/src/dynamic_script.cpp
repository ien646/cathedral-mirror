#include <cathedral/script/dynamic_script.hpp>

#include <cathedral/engine/nodes/camera2d_node.hpp>
#include <cathedral/engine/nodes/camera3d_node.hpp>
#include <cathedral/engine/nodes/directional_light_node.hpp>
#include <cathedral/engine/nodes/mesh3d_node.hpp>
#include <cathedral/engine/nodes/point_light_node.hpp>
#include <cathedral/engine/nodes/text_node.hpp>
#include <cathedral/engine/scene.hpp>

#include <cathedral/script/global_state.hpp>

namespace cathedral::script
{
    constexpr auto TICK_FUNCTION_NAME = "_Tick";
    constexpr auto EDITOR_TICK_FUNCTION_NAME = "_Editor_Tick";
    constexpr auto INIT_FUNCTION_NAME = "_Init";
    constexpr auto TEARDOWN_FUNCTION_NAME = "_Teardown";

    namespace
    {
        template <typename... TArgs>
        void node_call(luabridge::LuaRef& func, engine::scene_object* node, engine::scene& scene, const std::string& name, TArgs&&... args)
        {
            CRITICAL_CHECK(!func.isNil(), "Attempt to call null function");
            CRITICAL_CHECK(func.isFunction(), "Attempt to call non function type");
            const char* err = nullptr;
            const luabridge::TypeResult<void> result = func.callWithHandler<void>(
                [&](lua_State* L) -> int {
                    err = luaL_tolstring(L, 1, nullptr);
                    luaL_traceback(L, L, err, 1);
                    return 1;
                },
                node,
                scene,
                std::forward<TArgs>(args)...);
            if (result.error())
            {
                log_error(std::format("{}: {}", name, result.error_cstr()));
                log_error(err);
            }
        }
    } // namespace

    dynamic_script::dynamic_script(global_state& gstate, std::string name, [[maybe_unused]] engine::scene& scene)
        : script(MOVE(name))
        , _state(gstate)
    {
        _environment = std::make_unique<luabridge::LuaRef>(gstate.create_environment());
        const bool set_ok = _environment->setField("__cathedral_script__", _name);
        CRITICAL_CHECK(set_ok, "Unable to set dynamic script environment variable");
    }

    void dynamic_script::init(engine::scene_object* node, engine::scene& scene)
    {
        if (_init.has_value())
        {
            if (!_initialized)
            {
                node_call(*_init, node, scene, _name);
                _initialized = true;
            }
        }
    }

    void dynamic_script::tick(engine::scene_object* node, engine::scene& scene, double deltatime)
    {
        if (_tick.has_value())
        {
            node_call(*_tick, node, scene, _name, deltatime);
        }
        _state.collect_garbage();
    }

    void dynamic_script::editor_tick(engine::scene_object* node, engine::scene& scene, double deltatime)
    {
        if (_editor_tick.has_value())
        {
            node_call(*_editor_tick, node, scene, _name, deltatime);
        }
        _state.collect_garbage();
    }

    void dynamic_script::teardown(engine::scene_object* node, engine::scene& scene)
    {
        if (_teardown.has_value())
        {
            node_call(*_teardown, node, scene, _name);
        }
    }

    void dynamic_script::set_source(std::string s)
    {
        _source = MOVE(s);
        lua_State* state = _environment->state();

        try
        {
            if (luaL_loadstring(state, _source.c_str()) != LUA_OK)
            {
                lua_pop(state, 1);
                log_error("Unable to load lua script chunk");
                _init = {};
                _tick = {};
                _editor_tick = {};
                _teardown = {};
                return;
            }

            _environment->push(state);
            if (!lua_setupvalue(state, -2, 1))
            {
                lua_pop(state, 1);
                lua_pop(state, 1);
                log_error("Unable to set lua script chunk environment");
                _init = {};
                _tick = {};
                _editor_tick = {};
                _teardown = {};
                return;
            }

            if (lua_pcall(state, 0, 0, 0) != LUA_OK)
            {
                log_error(std::format("Failure running lua script: {}", lua_tostring(state, -1)));
                lua_pop(state, 1);
                _init = {};
                _tick = {};
                _editor_tick = {};
                _teardown = {};
                return;
            }
        }
        catch (const std::exception& e)
        {
            log_error(e.what());
            return;
        }

        luabridge::LuaRef init = (*_environment)[INIT_FUNCTION_NAME];
        luabridge::LuaRef tick = (*_environment)[TICK_FUNCTION_NAME];
        luabridge::LuaRef etick = (*_environment)[EDITOR_TICK_FUNCTION_NAME];
        luabridge::LuaRef teardown = (*_environment)[TEARDOWN_FUNCTION_NAME];

        // Evaluate the existence of script specific functions
        _init = !init.isNil() && init.isFunction() ? init : std::optional<luabridge::LuaRef>();
        _tick = !tick.isNil() && tick.isFunction() ? tick : std::optional<luabridge::LuaRef>();
        _editor_tick = !etick.isNil() && etick.isFunction() ? etick : std::optional<luabridge::LuaRef>();
        _teardown = !teardown.isNil() && teardown.isFunction() ? teardown : std::optional<luabridge::LuaRef>();
    }

    bool dynamic_script::initialized() const
    {
        return _initialized;
    }

    bool dynamic_script::enabled() const
    {
        return _enabled;
    }

    void dynamic_script::set_enabled(const bool enabled)
    {
        _enabled = enabled;
    }
} // namespace cathedral::script