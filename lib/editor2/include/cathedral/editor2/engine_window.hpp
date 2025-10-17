#pragma once

#include "settings.hpp"

#include <cathedral/engine/renderer.hpp>
#include <cathedral/gfx/swapchain.hpp>
#include <cathedral/gfx/vulkan_context.hpp>
#include <cathedral/sdl/keyboard.hpp>
#include <cathedral/sdl/mouse.hpp>
#include <cathedral/sdl/window.hpp>

#include <imgui.h>
#include <memory>

namespace cathedral::editor2
{
    class engine_window
    {
    public:
        engine_window(
            const std::string& name,
            size_t initial_width,
            size_t initial_height,
            std::shared_ptr<settings> settings);

        ~engine_window();

        template <typename TCallable>
        void tick(const TCallable& tick_call)
        {
            pre_tick();
            tick_call();
            post_tick();
        }

        bool keep_open() const { return _keep_open; }

        void prepare_to_close() const;

        void show() const;
        void hide() const;

        void close();

        engine::renderer& renderer() const { return *_renderer; }

        sdl::window& window() { return _window; }

        ImGuiContext* get_imgui_context() const { return _imgui_context; }

        std::shared_ptr<engine::engine_settings_interface> engine_settings() const { return _engine_settings; }

        std::shared_ptr<editor_settings_interface> editor_settings() const { return _editor_settings; }

    private:
        bool _keep_open = true;
        sdl::window _window;
        std::shared_ptr<sdl::keyboard_input> _kb;
        std::shared_ptr<sdl::mouse_input> _mouse;
        ImGuiContext* _imgui_context;
        ImFont* _imgui_font;
        std::shared_ptr<editor_settings_interface> _editor_settings;
        std::shared_ptr<engine::engine_settings_interface> _engine_settings;
        std::unique_ptr<gfx::vulkan_context> _vkctx;
        std::unique_ptr<gfx::swapchain> _swapchain;
        std::unique_ptr<engine::renderer> _renderer;
        std::unique_ptr<settings::subscription> _msaa_subscription;

        void pre_tick();
        void post_tick();

        void init_vkctx();
        void init_swapchain();
        void init_renderer();

        void init_vulkan_imgui();

        void process_sdl_event(SDL_Event& event);
    };
} // namespace cathedral::editor2