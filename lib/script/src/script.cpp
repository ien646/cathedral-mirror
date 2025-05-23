#include <cathedral/script/script.hpp>

#include <cathedral/engine/scene.hpp>

namespace cathedral::script
{
    script::script(state& s, engine::scene& scene)
        : _state(s)
    {
        _env.set("deltatime", scene.last_deltatime());
        _env.set("scene", &scene);
    }

    void script::init(engine::scene& scene)
    {
        if (!_init_script.empty())
        {
            _state.script(_init_script, _env);
        }
    }

    void script::tick(engine::scene& scene, double deltatime)
    {
        if (!_tick_script.empty())
        {
            _state.script(_tick_script, _env);
        }
    }

    void script::editor_tick(engine::scene& scene, double deltatime)
    {
        if (_editor_tick_script.empty())
        {
            _state.script(_editor_tick_script, _env);
        }
    }

    void script::teardown(engine::scene& scene)
    {
        if (_teardown_script.empty())
        {
            _state.script(_teardown_script, _env);
        }
    }

    void script::set_init_script(std::string s)
    {
        _init_script = std::move(s);
    }

    void script::set_tick_script(std::string s)
    {
        _tick_script = std::move(s);
    }

    void script::set_editor_tick_script(std::string s)
    {
        _editor_tick_script = std::move(s);
    }

    void script::set_teardown_script(std::string s)
    {
        _teardown_script = std::move(s);
    }
} // namespace cathedral::script::script