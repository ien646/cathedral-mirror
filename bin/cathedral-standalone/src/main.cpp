#include <cathedral/core.hpp>
#include <cathedral/gfx/vulkan_context.hpp>
#include <cathedral/sdl/input.hpp>
#include <cathedral/sdl/window.hpp>

#include <cathedral/engine/renderer.hpp>
#include <cathedral/engine/scene.hpp>

#include <cathedral/project/project.hpp>

int main(int argc, char* argv[])
{
    cathedral::sdl::window window("cathedral-standalone", 800, 600);

    cathedral::gfx::vulkan_context_args vkctx_args;
    vkctx_args.instance_extensions = window.get_vulkan_instance_extensions();
    vkctx_args.surface_retriever = [&](const vk::Instance instance) { return window.create_surface(instance); };
    vkctx_args.surface_size_retriever = [&] { return window.get_size(); };
    vkctx_args.validation_layers = cathedral::is_debug_build();

    cathedral::gfx::vulkan_context vkctx(vkctx_args);

    cathedral::gfx::swapchain swapchain(vkctx, vk::PresentModeKHR::eFifo);

    cathedral::engine::renderer_args renderer_args;
    renderer_args.swapchain = &swapchain;

    cathedral::engine::renderer renderer(renderer_args);

    cathedral::project::project project;
    const auto load_project_result = project.load_project("/home/ien/Projects/cathedral/test-project");
    CRITICAL_CHECK(load_project_result == cathedral::project::load_project_status::OK, "Failed to load project");

    cathedral::sdl::input input;
    auto kb = std::make_shared<cathedral::sdl::keyboard_input>(input);
    auto mouse = std::make_shared<cathedral::sdl::mouse_input>(input);

    project.set_scene_load_callback([&](cathedral::engine::scene& scene) {
        scene.set_keyboard_input_interface(kb);
        scene.set_mouse_input_interface(mouse);
    });
    auto scene = project.load_scene("input_test", &renderer);

    while (true)
    {
        SDL_Event event;
        while (SDL_PollEvent(&event))
        {
            switch (event.type)
            {
            case SDL_EVENT_KEY_DOWN:
                kb->press_key(static_cast<cathedral::engine::keyboard_keycode>(event.key.key));
                break;
            case SDL_EVENT_KEY_UP:
                kb->release_key(static_cast<cathedral::engine::keyboard_keycode>(event.key.key));
                break;
            case SDL_EVENT_MOUSE_BUTTON_DOWN:
                mouse->press_button(static_cast<cathedral::engine::mouse_button>(event.button.button));
                break;
            case SDL_EVENT_MOUSE_BUTTON_UP:
                mouse->release_button(static_cast<cathedral::engine::mouse_button>(event.button.button));
                break;
            case SDL_EVENT_MOUSE_MOTION:
                mouse->set_mouse_position(glm::ivec2(event.motion.x, event.motion.y));
                mouse->set_mouse_delta(glm::ivec2(event.motion.xrel, event.motion.yrel));
                break;
            default:
                break;
            }
        }
        scene.tick([&](const double deltatime) {

        });
    }
}