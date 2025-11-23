#include "cathedral/gfx/check.hpp"

#include <cathedral/editor/engine_window.hpp>

#include <cathedral/core.hpp>
#include <cathedral/editor/style.hpp>
#include <cathedral/engine/engine_settings.hpp>
#include <cathedral/engine/input.hpp>
#include <cathedral/sdl/event.hpp>

#include <backends/imgui_impl_sdl3.h>
#include <backends/imgui_impl_vulkan.h>

#include <magic_enum.hpp>

#include <battery/embed.hpp>

extern ImGuiKey ImGui_ImplSDL3_KeyEventToImGuiKey(SDL_Keycode keycode, SDL_Scancode scancode);

namespace cathedral::editor
{
    engine_window::engine_window(
        const std::string& name,
        const size_t initial_width,
        const size_t initial_height,
        std::shared_ptr<settings> settings)
        : _window(name, initial_width, initial_height)
        , _imgui_context(ImGui::CreateContext())
        , _editor_settings(std::make_shared<editor_settings_interface>(settings, "cathedral::editor::"))
        , _engine_settings(std::make_shared<engine::engine_settings_interface>(settings, "cathedral::engine::"))
    {
        ImGui::SetCurrentContext(_imgui_context);

        init_vkctx();
        init_swapchain();
        init_renderer();

        _kb = std::make_shared<sdl::keyboard_input>();
        _mouse = std::make_shared<sdl::mouse_input>();

        init_vulkan_imgui();

        _msaa_samples_subscription = _engine_settings->subscribe(engine::engine_setting::MSAA_SAMPLES, [this](auto&&) {
            recreate_imgui_context();
        });

        _msaa_sample_shading_subscription = _engine_settings->subscribe(
            engine::engine_setting::MSAA_SAMPLE_SHADING,
            [this](auto&&) { recreate_imgui_context(); });

        _vsync_subscription = _engine_settings->subscribe(engine::engine_setting::VSYNC_ENABLED, [this](auto&&) {
            recreate_imgui_context();
        });

        _vsync_mailbox_subscription = _engine_settings->subscribe(engine::engine_setting::VSYNC_MAILBOX, [this](auto&&) {
            recreate_imgui_context();
        });

        _window.set_event_handler([this](auto& ev) { process_sdl_event(ev); });
        _window.show();
    }

    engine_window::~engine_window()
    {
        prepare_to_close();

        ImGui::SetCurrentContext(_imgui_context);

        ImGui_ImplVulkan_Shutdown();
        ImGui_ImplSDL3_Shutdown();
        ImGui::DestroyContext(_imgui_context);
    }

    void engine_window::prepare_to_close() const
    {
        CATHEDRAL_VK_RESULT_CHECKED(_renderer->vkctx().device().waitIdle());
    }

    void engine_window::show() const
    {
        _window.show();
    }

    void engine_window::hide() const
    {
        _window.hide();
    }

    void engine_window::close()
    {
        _keep_open = false;
        prepare_to_close();
    }

    void engine_window::set_title(const std::string& text) const
    {
        _window.set_title(text);
    }

    void engine_window::recreate_imgui_context()
    {
        _needs_recreate_context = true;
    }

    void engine_window::pre_tick()
    {
        ImGui::SetCurrentContext(_imgui_context);

        ImGui_ImplVulkan_NewFrame();
        ImGui_ImplSDL3_NewFrame();
        ImGui::NewFrame();
    }

    void engine_window::post_tick()
    {
        if (_needs_recreate_context)
        {
            ImGui::SetCurrentContext(_imgui_context);
            ImGui::Render();

            ImGui_ImplVulkan_Shutdown();
            ImGui_ImplSDL3_Shutdown();

            init_vulkan_imgui();

            _needs_recreate_context = false;
            return;
        }

        ImGui::Render();
        ImDrawData* draw_data = ImGui::GetDrawData();
        ImGui_ImplVulkan_RenderDrawData(draw_data, _renderer->render_cmdbuff(engine::render_domain::OVERLAY));

        _kb->tick();
        _mouse->tick();
    }

    void engine_window::init_vkctx()
    {
        gfx::vulkan_context_args vkctx_args;
        vkctx_args.instance_extensions = _window.get_vulkan_instance_extensions();
        vkctx_args.surface_retriever = [this](const vk::Instance vkinst) -> vk::SurfaceKHR {
            return _window.create_surface(vkinst);
        };
        vkctx_args.surface_size_retriever = [this] { return _window.get_pixel_size(); };
        vkctx_args.validation_layers = is_debug_build();
        _vkctx = std::make_unique<gfx::vulkan_context>(vkctx_args);
    }

    void engine_window::init_swapchain()
    {
        _swapchain = std::make_unique<gfx::swapchain>(*_vkctx, vk::PresentModeKHR::eMailbox);
    }

    void engine_window::init_renderer()
    {
        engine::renderer_args args;
        args.swapchain = _swapchain.get();
        args.engine_settings = _engine_settings;

        _renderer = std::make_unique<engine::renderer>(args);
    }

    void engine_window::init_vulkan_imgui()
    {
        ImGui::SetCurrentContext(_imgui_context);
        ImGui::GetIO().ConfigFlags |= ImGuiConfigFlags_DockingEnable;

        setup_imgui_style();

        auto font_scale = _editor_settings->get(editor_setting::TEXT_SCALE);
        if (font_scale.type() != setting_type::DOUBLE)
        {
            log_error("Setting cathedral::editor::font_scale has invalid type");
        }
        else
        {
            ImGui::GetIO().FontGlobalScale = static_cast<float>(font_scale.as_double());
        }

        auto font = b::embed<"editor/fonts/JetBrainsMono-Regular.ttf">().vec();
        ImFontConfig font_config;
        font_config.FontDataOwnedByAtlas = false;
        _imgui_font =
            ImGui::GetIO().Fonts->AddFontFromMemoryTTF(font.data(), static_cast<int>(font.size()), 16, &font_config);

        ImGui_ImplSDL3_InitForVulkan(_window.get_handle());

        const auto [enum_values, current_value] = _engine_settings->get(engine::engine_setting::MSAA_SAMPLES).as_enum();
        const auto str = enum_values.at(current_value);
        const auto samples = static_cast<VkSampleCountFlagBits>(std::stoi(str));

        ImGui_ImplVulkan_InitInfo vk_init_info = {};
        vk_init_info.Allocator = nullptr;
        vk_init_info.ApiVersion = VK_API_VERSION_1_3;
        vk_init_info.CheckVkResultFn = [](const VkResult result) {
            CRITICAL_CHECK(
                result == VkResult::VK_SUCCESS,
                std::format("imgui vulkan result check failure! -> ", magic_enum::enum_name(result)));
        };
        vk_init_info.DescriptorPool = _vkctx->descriptor_pool();
        vk_init_info.Device = _vkctx->device();
        vk_init_info.ImageCount = static_cast<uint32_t>(_swapchain->image_count());
        vk_init_info.Instance = _vkctx->instance();
        vk_init_info.MinImageCount = static_cast<uint32_t>(_swapchain->image_count());
        vk_init_info.PhysicalDevice = _vkctx->physdev();
        vk_init_info.PipelineCache = _vkctx->pipeline_cache();
        vk_init_info.Queue = _vkctx->graphics_queue();
        vk_init_info.QueueFamily = _vkctx->graphics_queue_family_index();
        vk_init_info.UseDynamicRendering = true;

        const auto color_format = static_cast<VkFormat>(_swapchain->swapchain_image_format());

        VkPipelineRenderingCreateInfoKHR pipeline_rendering_create_info = {};
        pipeline_rendering_create_info.sType = VK_STRUCTURE_TYPE_PIPELINE_RENDERING_CREATE_INFO_KHR;
        pipeline_rendering_create_info.colorAttachmentCount = 1;
        pipeline_rendering_create_info.depthAttachmentFormat = static_cast<VkFormat>(
            _renderer->depthstencil_attachment().format());
        pipeline_rendering_create_info.pColorAttachmentFormats = &color_format;
        pipeline_rendering_create_info.stencilAttachmentFormat = static_cast<VkFormat>(
            _renderer->depthstencil_attachment().format());
        pipeline_rendering_create_info.viewMask = 0;

        vk_init_info.PipelineInfoMain.MSAASamples = samples;
        vk_init_info.PipelineInfoMain.PipelineRenderingCreateInfo = pipeline_rendering_create_info;

        [[maybe_unused]] const bool ok = ImGui_ImplVulkan_Init(&vk_init_info);
        CRITICAL_CHECK(ok, "Failure initializing imgui vulkan backend");
    }

    void engine_window::process_sdl_event(SDL_Event& event)
    {
        ImGui::SetCurrentContext(_imgui_context);

        // Workaround for io.InputQueueCharacters on SDL3 backend
        if (ImGui::GetIO().WantTextInput)
        {
            SDL_StartTextInput(_window.get_handle());
        }
        else
        {
            SDL_StopTextInput(window().get_handle());
        }

        ImGui_ImplSDL3_ProcessEvent(&event);

        const bool key_event = (event.type == SDL_EVENT_KEY_DOWN) || (event.type == SDL_EVENT_KEY_UP);
        const bool text_event = (event.type == SDL_EVENT_TEXT_INPUT) || (event.type == SDL_EVENT_TEXT_EDITING);
        if ((ImGui::GetIO().WantCaptureKeyboard && key_event) || (ImGui::GetIO().WantTextInput && text_event))
        {
            return;
        }

        if (ImGui::GetIO().WantCaptureMouse
            && ((event.type == SDL_EVENT_MOUSE_BUTTON_DOWN)
                || (event.type == SDL_EVENT_MOUSE_BUTTON_UP)
                || (event.type == SDL_EVENT_MOUSE_MOTION)))
        {
            return;
        }

        switch (event.type)
        {
        case SDL_EVENT_KEY_DOWN:
            _kb->press_key(static_cast<engine::keyboard_keycode>(event.key.key));
            break;
        case SDL_EVENT_KEY_UP:
            _kb->release_key(static_cast<engine::keyboard_keycode>(event.key.key));
            break;
        case SDL_EVENT_MOUSE_BUTTON_DOWN:
            _mouse->press_button(static_cast<engine::mouse_button>(event.button.button));
            break;
        case SDL_EVENT_MOUSE_BUTTON_UP:
            _mouse->release_button(static_cast<engine::mouse_button>(event.button.button));
            break;
        case SDL_EVENT_MOUSE_MOTION:
            _mouse->set_mouse_position(glm::ivec2(event.motion.x, event.motion.y));
            _mouse->set_mouse_delta(glm::ivec2(event.motion.xrel, event.motion.yrel));
            break;
        case SDL_EVENT_WINDOW_RESIZED:
            std::println("Window resized");
            _swapchain->recreate();
            _renderer->recreate_swapchain_dependent_resources();
            recreate_imgui_context();
            break;
        case SDL_EVENT_WINDOW_CLOSE_REQUESTED:
            _keep_open = false;
            break;
        default:
            break;
        }
    }
} // namespace cathedral::editor