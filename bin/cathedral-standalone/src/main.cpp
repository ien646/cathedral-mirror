#include <cathedral/core.hpp>
#include <cathedral/engine/renderer.hpp>
#include <cathedral/engine/scene.hpp>
#include <cathedral/gfx/vulkan_context.hpp>
#include <cathedral/memory.hpp>
#include <cathedral/project/project.hpp>
#include <cathedral/sdl/keyboard.hpp>
#include <cathedral/sdl/mouse.hpp>
#include <cathedral/sdl/window.hpp>

int main(int argc, char* argv[])
{
    if (argc < 3)
    {
        std::println("Not enough arguments");
        std::println("Args:");
        for (int i = 0; i < argc; ++i)
        {
            std::println("{}", argv[i]);
        }
        return -1;
    }
    const std::string project_path = argv[1];
    const std::string initial_scene = argv[2];

    std::println("Initializing scratch memory");
    cathedral::init_scratch_memory();

    std::println("Initializing SDL window");
    cathedral::sdl::window window("cathedral-standalone", 1920, 1080);

    cathedral::gfx::vulkan_context_args vkctx_args;
    vkctx_args.instance_extensions = window.get_vulkan_instance_extensions();
    vkctx_args.surface_retriever = [&](const vk::Instance instance) { return window.create_surface(instance); };
    vkctx_args.surface_size_retriever = [&] { return window.get_size(); };
    vkctx_args.validation_layers = cathedral::is_debug_build();

    cathedral::gfx::vulkan_context vkctx(vkctx_args);

    cathedral::gfx::swapchain swapchain(vkctx, vk::PresentModeKHR::eFifo);

    std::println("Loading project at path '{}'", project_path);
    cathedral::project::project project;
    const auto load_project_result = project.load_project(project_path);
    CRITICAL_CHECK(load_project_result == cathedral::project::load_project_status::OK, "Failed to load project");

    cathedral::engine::renderer_args renderer_args;
    renderer_args.engine_settings = project.get_engine_settings();
    renderer_args.swapchain = &swapchain;

    cathedral::engine::renderer renderer(renderer_args);

    auto kb = std::make_shared<cathedral::sdl::keyboard_input>();
    auto mouse = std::make_shared<cathedral::sdl::mouse_input>();

    project.set_scene_load_callback([&](cathedral::engine::scene& scene) {
        scene.set_keyboard_input_interface(kb);
        scene.set_mouse_input_interface(mouse);
    });

    std::println("Loading scene '{}'", initial_scene);
    auto scene = project.load_scene(initial_scene, &renderer);

    bool keep_running = true;
    while (keep_running)
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
            case SDL_EVENT_WINDOW_CLOSE_REQUESTED:
                keep_running = false;
                break;
            default:
                break;
            }
        }
        scene.tick([&](const double deltatime) {
            for (const auto& line : cathedral::get_global_log_database().take_log_lines())
            {
                std::println("[{}]: {}", magic_enum::enum_name(line.level), line.message);
            }
        });
        cathedral::flush_scratch_memory();
    }
}