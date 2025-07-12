#include "cathedral/sdl/keyboard.hpp"
#include "cathedral/sdl/mouse.hpp"

#include <cathedral/editor2/editor_window.hpp>

#include <cathedral/editor2/menubar.hpp>

#include <cathedral/engine/renderer.hpp>
#include <cathedral/engine/scene.hpp>
#include <cathedral/project/project.hpp>
#include <cathedral/sdl/window.hpp>

#include <backends/imgui_impl_sdl3.h>
#include <backends/imgui_impl_vulkan.h>
#include <imgui.h>

#include <SDL3/SDL_events.h>

namespace cathedral::editor2
{
    editor_window::editor_window()
    {
        _window = std::make_unique<sdl::window>("cathedral-standalone", 1920, 1080);

        gfx::vulkan_context_args vkctx_args;
        vkctx_args.instance_extensions = _window->get_vulkan_instance_extensions();
        vkctx_args.surface_retriever = [&](const vk::Instance instance) { return _window->create_surface(instance); };
        vkctx_args.surface_size_retriever = [&] { return _window->get_size(); };
        vkctx_args.validation_layers = false; // cathedral::is_debug_build();

        _vkctx = std::make_unique<gfx::vulkan_context>(vkctx_args);

        _swapchain = std::make_unique<gfx::swapchain>(*_vkctx, vk::PresentModeKHR::eImmediate);

        engine::renderer_args renderer_args;
        renderer_args.swapchain = _swapchain.get();

        _renderer = std::make_unique<engine::renderer>(renderer_args);

        _project = std::make_unique<project::project>();
        const auto load_project_result = _project->load_project("/home/ien/Projects/cathedral/test-project");
        CRITICAL_CHECK(load_project_result == cathedral::project::load_project_status::OK, "Failed to load project");

        _keyboard_input = std::make_shared<sdl::keyboard_input>();
        _mouse_input = std::make_shared<sdl::mouse_input>();

        _project->set_scene_load_callback([&](engine::scene& scene) {
            scene.set_keyboard_input_interface(_keyboard_input);
            scene.set_mouse_input_interface(_mouse_input);
        });
        _scene = std::make_shared<engine::scene>(_project->load_scene("monki_ser2", _renderer.get()));

        init_imgui();
        init_ui();
    }

    void editor_window::init_imgui()
    {
        ImGui::CreateContext();
        ImGui_ImplSDL3_InitForVulkan(_window->get_handle());

        ImGui_ImplVulkan_InitInfo vulkan_init_info = {};
        vulkan_init_info.ApiVersion = VK_API_VERSION_1_3;
        vulkan_init_info.DescriptorPool = _renderer->vkctx().descriptor_pool();
        vulkan_init_info.Device = _renderer->vkctx().device();
        vulkan_init_info.Instance = _renderer->vkctx().instance();
        vulkan_init_info.ImageCount = _renderer->swapchain().image_count();
        vulkan_init_info.MinImageCount = _renderer->swapchain().image_count();
        vulkan_init_info.MSAASamples = VK_SAMPLE_COUNT_1_BIT;
        vulkan_init_info.PhysicalDevice = _renderer->vkctx().physdev();
        vulkan_init_info.Queue = _renderer->vkctx().graphics_queue();
        vulkan_init_info.QueueFamily = _renderer->vkctx().graphics_queue_family_index();
        vulkan_init_info.UseDynamicRendering = true;
        vulkan_init_info.CheckVkResultFn = [](VkResult result) {
            if (result != VK_SUCCESS)
            {
                CRITICAL_ERROR("IMGUI ERROR");
            }
        };

        const auto color_format = static_cast<VkFormat>(_renderer->swapchain().swapchain_image_format());
        vulkan_init_info.PipelineRenderingCreateInfo = {};
        vulkan_init_info.PipelineRenderingCreateInfo.colorAttachmentCount = 1;
        vulkan_init_info.PipelineRenderingCreateInfo.pColorAttachmentFormats = &color_format;
        vulkan_init_info.PipelineRenderingCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_RENDERING_CREATE_INFO_KHR;
        vulkan_init_info.PipelineRenderingCreateInfo.pNext = nullptr;

        ImGui_ImplVulkan_Init(&vulkan_init_info);

        const auto scale = SDL_GetWindowDisplayScale(_window->get_handle());

        ImGui::StyleColorsDark();
        ImGui::GetStyle().FontScaleDpi = scale;
        ImGui::GetStyle().FontScaleMain = 0.5F;
    }

    void editor_window::init_ui()
    {
        _menubar = std::make_unique<menubar>();

        _menubar->callbacks().close = [this] { _should_close = true; };
    }

    void editor_window::tick()
    {
        SDL_Event event;
        while (SDL_PollEvent(&event))
        {
            ImGui_ImplSDL3_ProcessEvent(&event);
            switch (event.type)
            {
            case SDL_EVENT_KEY_DOWN:
                _keyboard_input->press_key(static_cast<engine::keyboard_keycode>(event.key.key));
                break;
            case SDL_EVENT_KEY_UP:
                _keyboard_input->release_key(static_cast<engine::keyboard_keycode>(event.key.key));
                break;
            case SDL_EVENT_MOUSE_BUTTON_DOWN:
                _mouse_input->press_button(static_cast<engine::mouse_button>(event.button.button));
                break;
            case SDL_EVENT_MOUSE_BUTTON_UP:
                _mouse_input->release_button(static_cast<engine::mouse_button>(event.button.button));
                break;
            case SDL_EVENT_MOUSE_MOTION:
                _mouse_input->set_mouse_position(glm::ivec2(event.motion.x, event.motion.y));
                _mouse_input->set_mouse_delta(glm::ivec2(event.motion.xrel, event.motion.yrel));
                break;
            case SDL_EVENT_WINDOW_CLOSE_REQUESTED:
                _should_close = true;
                break;
            default:
                break;
            }
        }
        _scene->tick([&]([[maybe_unused]] const double deltatime) {
            ImGui_ImplVulkan_NewFrame();
            ImGui_ImplSDL3_NewFrame();
            ImGui::NewFrame();

            _menubar->tick();

            ImGui::Render();
            auto* draw_data = ImGui::GetDrawData();
            ImGui_ImplVulkan_RenderDrawData(draw_data, _renderer->render_cmdbuff(engine::render_cmdbuff_type::OVERLAY));
        });
    }

    bool editor_window::should_close() const
    {
        return _should_close;
    }
} // namespace cathedral::editor2