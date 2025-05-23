#pragma once

#include <cathedral/script/state.hpp>

#include <cathedral/engine/script.hpp>

namespace cathedral::script
{
    class script final : public engine::script
    {
    public:
        script(state& s, engine::scene& scene);
        void init(engine::scene& scene) override;
        void tick(engine::scene& scene, double deltatime) override;
        void editor_tick(engine::scene& scene, double deltatime) override;
        void teardown(engine::scene& scene) override;

        void set_init_script(std::string s);
        void set_tick_script(std::string s);
        void set_editor_tick_script(std::string s);
        void set_teardown_script(std::string s);

    private:
        state& _state;
        sol::environment _env;
        std::string _init_script;
        std::string _tick_script;
        std::string _editor_tick_script;
        std::string _teardown_script;
    };
} // namespace cathedral::script::script