#include <cathedral/script/dynamic_script.hpp>

#include <cathedral/engine/scene.hpp>

namespace cathedral::script
{
    dynamic_script::dynamic_script(state& s, engine::scene& scene)
        : _state(s)
    {
        _env = sol::environment(_state, sol::create, _state.globals());
    }

    void dynamic_script::init(engine::scene& scene)
    {
        if (_init.has_value())
        {
            _init->call<void>(scene);
        }
    }

    void dynamic_script::tick(engine::scene& scene, double deltatime)
    {
        if (_tick.has_value())
        {
            _tick->call<void>(scene, deltatime);
        }
    }

    void dynamic_script::editor_tick(engine::scene& scene, double deltatime)
    {
        if (_editor_tick.has_value())
        {
            _editor_tick->call<void>(scene, deltatime);
        }
    }

    void dynamic_script::teardown(engine::scene& scene)
    {
        if (_teardown.has_value())
        {
            _teardown->call<void>(scene);
        }
    }

    void dynamic_script::set_source(std::string s)
    {
        _source = std::move(s);

        // Reset environment and run user script in clean environment
        _env = sol::environment(_state, sol::create, _state.globals());
        _state.script(_source, _env);

        // Evaluate the existence of script specific functions
        const sol::function init_func = _env["init"];
        const sol::function tick_func = _env["tick"];
        const sol::function editor_tick_func = _env["editor_tick"];
        const sol::function teardown_func = _env["teardown"];

        _init = init_func.valid() ? init_func : std::optional<sol::function>{};
        _tick = tick_func.valid() ? tick_func : std::optional<sol::function>{};
        _editor_tick = editor_tick_func.valid() ? editor_tick_func : std::optional<sol::function>{};
        _teardown = teardown_func.valid() ? teardown_func : std::optional<sol::function>{};
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