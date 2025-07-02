#pragma once

#include <SDL3/SDL.h>

#include <vulkan/vulkan.hpp>

#include <glm/vec2.hpp>

#include <string>
#include <vector>

namespace cathedral::sdl
{
    class window
    {
    public:
        window(const std::string& application_name, size_t initial_width, size_t initial_height);

        std::vector<const char*> get_vulkan_instance_extensions();
        vk::SurfaceKHR create_surface(vk::Instance instance) const;

        glm::ivec2 get_size() const;

        void show() const;
        void hide() const;

    private:
        SDL_Window* _window = nullptr;
    };
} // namespace cathedral::sdl