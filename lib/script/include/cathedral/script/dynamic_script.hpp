#pragma once

#include <cathedral/script/state.hpp>

#include <cathedral/engine/script.hpp>

#include <sol/sol.hpp>

namespace cathedral::script
{
    class dynamic_script final : public engine::script
    {
    public:
        dynamic_script(state& s, engine::scene& scene);
        void init(engine::scene& scene) override;
        void tick(engine::scene& scene, double deltatime) override;
        void editor_tick(engine::scene& scene, double deltatime) override;
        void teardown(engine::scene& scene) override;

        void set_source(std::string s);

        bool initialized() const override;
        bool enabled() const override;
        void set_enabled(bool enabled) override;

    private:
        state& _state;
        sol::environment _env;
        bool _initialized = false;
        bool _enabled = true;
        std::string _source;

        std::optional<sol::function> _init;
        std::optional<sol::function> _tick;
        std::optional<sol::function> _editor_tick;
        std::optional<sol::function> _teardown;
    };
} // namespace cathedral::script