#include <cathedral/sdl/window.hpp>

#include <cathedral/core.hpp>

#include <SDL3/SDL.h>
#include <SDL3/SDL_video.h>
#include <SDL3/SDL_vulkan.h>

namespace cathedral::sdl
{
    window::window(const std::string& application_name, const size_t initial_width, const size_t initial_height)
    {
        SDL_Init(SDL_INIT_VIDEO);

        _window = SDL_CreateWindow(
            application_name.c_str(),
            static_cast<int>(initial_width),
            static_cast<int>(initial_height),
            SDL_WINDOW_VULKAN | SDL_WINDOW_HIGH_PIXEL_DENSITY | SDL_WINDOW_RESIZABLE);

        if (_window == nullptr)
        {
            CRITICAL_ERROR(std::format("Unable to create SDL window -> {}", SDL_GetError()));
            return;
        }
    }

    std::vector<const char*> window::get_vulkan_instance_extensions()
    {
        uint32_t vk_inst_extension_count = 0;
        const auto* const extensions = SDL_Vulkan_GetInstanceExtensions(&vk_inst_extension_count);

        std::vector<const char*> result;
        result.reserve(vk_inst_extension_count);

        for (uint32_t i = 0; i < vk_inst_extension_count; ++i)
        {
            result.emplace_back(extensions[i]);
        }

        return result;
    }

    vk::SurfaceKHR window::create_surface(const vk::Instance instance) const
    {
        VkSurfaceKHR surface;
        SDL_Vulkan_CreateSurface(_window, instance, nullptr, &surface);
        return surface;
    }

    glm::ivec2 window::get_size() const
    {
        int w;
        int h;
        SDL_GetWindowSize(_window, &w, &h);
        const auto scale = SDL_GetWindowDisplayScale(_window);
        return { static_cast<float>(w) * scale, static_cast<float>(h) * scale };
    }

    glm::ivec2 window::get_pixel_size() const
    {
        int w;
        int h;
        SDL_GetWindowSizeInPixels(_window, &w, &h);
        return { w, h };
    }

    void window::show() const
    {
        SDL_ShowWindow(_window);
    }

    void window::hide() const
    {
        SDL_HideWindow(_window);
    }
} // namespace cathedral::sdl