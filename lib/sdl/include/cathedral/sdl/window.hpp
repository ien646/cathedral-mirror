#pragma once

#include <SDL3/SDL.h>

#include <cathedral/gfx/vk_hpp.hpp>

#include <glm/vec2.hpp>

#include <functional>
#include <string>
#include <vector>

namespace cathedral::sdl
{
    class window
    {
    public:
        window(const std::string& application_name, size_t initial_width, size_t initial_height);
        ~window();

        std::vector<const char*> get_vulkan_instance_extensions();
        vk::SurfaceKHR create_surface(vk::Instance instance) const;

        glm::ivec2 get_size() const;
        glm::ivec2 get_pixel_size() const;

        float get_scale() const;

        void show() const;
        void hide() const;

        void set_title(const std::string& title) const;

        SDL_Window* get_handle() const { return _window; }

        void set_event_handler(std::function<void(SDL_Event& event)> handler);

        void handle_event(SDL_Event& event) const;

    private:
        SDL_Window* _window = nullptr;
        std::function<void(SDL_Event& event)> _event_handler;
    };
} // namespace cathedral::sdl