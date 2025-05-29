#include <cathedral/script/dynamic_script.hpp>

#include <cathedral/engine/scene.hpp>

namespace cathedral::script
{
    dynamic_script::dynamic_script(state& s, engine::scene& scene)
        : _state(s)
    {
        _env.set("deltatime", scene.last_deltatime());
        _env.set("scene", &scene);
    }

    void dynamic_script::init(engine::scene& scene)
    {
        if (!_init_script.empty())
        {
            _state.script(_init_script, _env);
        }
    }

    void dynamic_script::tick(engine::scene& scene, double deltatime)
    {
        if (!_tick_script.empty())
        {
            _state.script(_tick_script, _env);
        }
    }

    void dynamic_script::editor_tick(engine::scene& scene, double deltatime)
    {
        if (_editor_tick_script.empty())
        {
            _state.script(_editor_tick_script, _env);
        }
    }

    void dynamic_script::teardown(engine::scene& scene)
    {
        if (_teardown_script.empty())
        {
            _state.script(_teardown_script, _env);
        }
    }

    void dynamic_script::set_init_script(std::string s)
    {
        _init_script = std::move(s);
    }

    void dynamic_script::set_tick_script(std::string s)
    {
        _tick_script = std::move(s);
    }

    void dynamic_script::set_editor_tick_script(std::string s)
    {
        _editor_tick_script = std::move(s);
    }

    void dynamic_script::set_teardown_script(std::string s)
    {
        _teardown_script = std::move(s);
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