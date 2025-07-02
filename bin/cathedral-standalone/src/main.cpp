#include <cathedral/core.hpp>
#include <cathedral/gfx/vulkan_context.hpp>
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

    auto scene = project.load_scene("monki_test", &renderer);

    while (true)
    {
        SDL_Event event;
        while (SDL_PollEvent(&event))
        {
        }
        scene.tick([&](const double deltatime) {

        });
    }
}