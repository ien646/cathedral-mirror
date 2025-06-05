#pragma once

#include <cathedral/engine/script.hpp>

namespace cathedral::engine
{
    template <typename TCallable>
    concept NativeScriptInitCallable =
        std::is_invocable_v<TCallable, scene_node*, scene&> || std::is_null_pointer_v<TCallable>;

    template <typename TCallable>
    concept NativeScriptTickCallable =
        std::is_invocable_v<TCallable, scene_node*, scene&, double> || std::is_null_pointer_v<TCallable>;

    template <typename TCallable>
    concept NativeScriptEditorTickCallable = NativeScriptTickCallable<TCallable>;

    template <typename TCallable>
    concept NativeScriptTeardownCallable = NativeScriptInitCallable<TCallable> || std::is_null_pointer_v<TCallable>;

    template <
        NativeScriptInitCallable TInit,
        NativeScriptTickCallable TTick,
        NativeScriptEditorTickCallable TEditorTick,
        NativeScriptTeardownCallable TTeardown>
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

        void init(scene_node* node, scene& scene) override
        {
            if constexpr (!std::is_null_pointer_v<TInit>)
            {
                if (!_initialized)
                {
                    _init(node, scene);
                    _initialized = true;
                }
            }
        }

        void tick(scene_node* node, scene& scene, double deltatime) override
        {
            if constexpr (!std::is_null_pointer_v<TTick>)
            {
                _tick(node, scene, deltatime);
            }
        }

        void editor_tick(scene_node* node, scene& scene, double deltatime) override
        {
            if constexpr (!std::is_null_pointer_v<TEditorTick>)
            {
                _editor_tick(node, scene, deltatime);
            }
        }

        void teardown(scene_node* node, scene& scene) override
        {
            if constexpr (!std::is_null_pointer_v<TTeardown>)
            {
                _teardown(node, scene);
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

    template<typename Init, typename Tick, typename EditorTick, typename Teardown>
    std::shared_ptr<script> make_native_script(Init init, Tick tick, EditorTick editor_tick, Teardown teardown)
    {
        using ns_type = native_script<Init, Tick, EditorTick, Teardown>;
        return std::make_shared<ns_type>(init, tick, editor_tick, teardown);
    }
} // namespace cathedral::engine