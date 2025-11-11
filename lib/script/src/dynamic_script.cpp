#include <cathedral/script/dynamic_script.hpp>

#include <cathedral/engine/nodes/camera2d_node.hpp>
#include <cathedral/engine/nodes/camera3d_node.hpp>
#include <cathedral/engine/nodes/directional_light_node.hpp>
#include <cathedral/engine/nodes/mesh3d_node.hpp>
#include <cathedral/engine/nodes/point_light_node.hpp>
#include <cathedral/engine/nodes/text_node.hpp>
#include <cathedral/engine/scene.hpp>

#include <cathedral/script/state.hpp>

namespace cathedral::script
{
    constexpr auto TICK_FUNCTION_NAME = "_Tick";
    constexpr auto EDITOR_TICK_FUNCTION_NAME = "_Editor_Tick";
    constexpr auto INIT_FUNCTION_NAME = "_Init";
    constexpr auto TEARDOWN_FUNCTION_NAME = "_Teardown";

    namespace
    {
        template <typename... TArgs>
        void node_call(sol::protected_function& func, engine::scene_node* node, engine::scene& scene, TArgs&&... args)
        {
            func.call<void>(node, scene, args...);
        }
    } // namespace

    dynamic_script::dynamic_script(std::string name, [[maybe_unused]] engine::scene& scene)
        : script(name)
        , _state(get_initial_state())
    {
        _state.set("__cathedral_script__", MOVE(name));
    }

    void dynamic_script::init(engine::scene_node* node, engine::scene& scene)
    {
        if (_init.has_value())
        {
            if (!_initialized)
            {
                node_call(*_init, node, scene);
                _initialized = true;
            }
        }
    }

    void dynamic_script::tick(engine::scene_node* node, engine::scene& scene, double deltatime)
    {
        if (_tick.has_value())
        {
            node_call(*_tick, node, scene, deltatime);
        }
        _state.collect_garbage();
    }

    void dynamic_script::editor_tick(engine::scene_node* node, engine::scene& scene, double deltatime)
    {
        if (_editor_tick.has_value())
        {
            node_call(*_editor_tick, node, scene, deltatime);
        }
        _state.collect_garbage();
    }

    void dynamic_script::teardown(engine::scene_node* node, engine::scene& scene)
    {
        if (_teardown.has_value())
        {
            node_call(*_teardown, node, scene);
        }
    }

    void dynamic_script::set_source(std::string s)
    {
        _source = MOVE(s);

        try
        {
            _state.safe_script(_source);
        }
        catch (const std::exception& e)
        {
            log_error(e.what());
            return;
        }

        // Evaluate the existence of script specific functions
        const sol::safe_function init_func = { _state[INIT_FUNCTION_NAME], _state["__cathedral_error_handler__"] };
        const sol::safe_function tick_func = { _state[TICK_FUNCTION_NAME], _state["__cathedral_error_handler__"] };
        const sol::safe_function editor_tick_func = { _state[EDITOR_TICK_FUNCTION_NAME], _state["__cathedral_error_handler__"] };
        const sol::safe_function teardown_func = { _state[TEARDOWN_FUNCTION_NAME], _state["__cathedral_error_handler__"] };

        _init = init_func.valid() ? init_func : std::optional<sol::safe_function>{};
        _tick = tick_func.valid() ? tick_func : std::optional<sol::safe_function>{};
        _editor_tick = editor_tick_func.valid() ? editor_tick_func : std::optional<sol::safe_function>{};
        _teardown = teardown_func.valid() ? teardown_func : std::optional<sol::safe_function>{};
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