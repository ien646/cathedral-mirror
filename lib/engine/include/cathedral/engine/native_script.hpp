#pragma once

#include <cathedral/engine/script.hpp>

namespace cathedral::engine
{
    template <typename TCallable>
    concept native_script_init_callable = requires(TCallable t, scene& scn) { t(scn); } || std::is_null_pointer_v<TCallable>;

    template <typename TCallable>
    concept native_script_tick_callable =
        requires(TCallable t, scene& scn, double delta) { t(scn, delta); } || std::is_null_pointer_v<TCallable>;

    template <typename TCallable>
    concept native_script_editor_tick_callable = native_script_tick_callable<TCallable>;

    template <typename TCallable>
    concept native_script_teardown_callable = native_script_init_callable<TCallable> || std::is_null_pointer_v<TCallable>;

    template <
        native_script_init_callable TInit,
        native_script_tick_callable TTick,
        native_script_editor_tick_callable TEditorTick,
        native_script_teardown_callable TTeardown>
    class native_script final : public script
    {
    public:
        native_script(TInit init, TTick tick, TEditorTick editor_tick, TTeardown teardown)
            : _init(init)
            , _tick(tick)
            , _editor_tick(editor_tick)
            , _teardown(teardown)
        {
        }

        void init(scene& scene) override
        {
            if constexpr (std::is_null_pointer_v<TInit>)
            {
                if (!_initialized)
                {
                    _init(scene);
                    _initialized = true;
                }
            }
        }

        void tick(scene& scene, double deltatime) override
        {
            if constexpr (std::is_null_pointer_v<TTick>)
            {
                _tick(scene, deltatime);
            }
        }

        void editor_tick(scene& scene, double deltatime) override
        {
            if constexpr (std::is_null_pointer_v<TEditorTick>)
            {
                _editor_tick(scene, deltatime);
            }
        }

        void teardown(scene& scene) override
        {
            if constexpr (std::is_null_pointer_v<TTeardown>)
            {
                _teardown(scene);
            }
        }

        bool initialized() const override { return _initialized; }

        bool enabled() const override { return _enabled; }

        void set_enabled(const bool enabled) override { _enabled = enabled; }

    private:
        bool _initialized = false;
        bool _enabled = true;
        const TInit _init;
        const TTick _tick;
        const TEditorTick _editor_tick;
        const TTeardown _teardown;
    };
} // namespace cathedral::engine