#pragma once

#include <cathedral/script/global_state.hpp>

#include <cathedral/engine/script.hpp>

// clang-format off
#include <lauxlib.h>
#include <LuaBridge/LuaBridge.h>
// clang-format on

namespace cathedral::script
{
    class dynamic_script final : public engine::script
    {
    public:
        dynamic_script(global_state& gstate, std::string name, engine::scene& scene);
        void init(engine::scene_node* node, engine::scene& scene) override;
        void tick(engine::scene_node* node, engine::scene& scene, double deltatime) override;
        void editor_tick(engine::scene_node* node, engine::scene& scene, double deltatime) override;
        void teardown(engine::scene_node* node, engine::scene& scene) override;

        void set_source(std::string s);

        bool initialized() const override;
        bool enabled() const override;
        void set_enabled(bool enabled) override;

    private:
        global_state& _state;
        std::unique_ptr<luabridge::LuaRef> _environment;
        bool _initialized = false;
        bool _enabled = true;
        std::string _source;

        std::optional<luabridge::LuaRef> _init;
        std::optional<luabridge::LuaRef> _tick;
        std::optional<luabridge::LuaRef> _editor_tick;
        std::optional<luabridge::LuaRef> _teardown;
    };
} // namespace cathedral::script