#include "cathedral/engine/node_filters.hpp"
#include "cathedral/engine/nodes/mesh3d_node.hpp"

#include <cathedral/editor2/editor_window.hpp>

#include <cathedral/editor2/dialogs/native_dialogs.hpp>
#include <cathedral/editor2/menubar.hpp>

#include <cathedral/engine/renderer.hpp>
#include <cathedral/engine/scene.hpp>
#include <cathedral/project/project.hpp>
#include <cathedral/sdl/keyboard.hpp>
#include <cathedral/sdl/mouse.hpp>
#include <cathedral/sdl/window.hpp>

#include <backends/imgui_impl_sdl3.h>
#include <backends/imgui_impl_vulkan.h>
#include <imgui.h>

#include <SDL3/SDL_events.h>

#include <battery/embed.hpp>

namespace cathedral::editor2
{
    editor_window::editor_window()
    {
        _window = std::make_unique<sdl::window>("cathedral-standalone", 1280, 720);

        gfx::vulkan_context_args vkctx_args;
        vkctx_args.instance_extensions = _window->get_vulkan_instance_extensions();
        vkctx_args.surface_retriever = [&](const vk::Instance instance) { return _window->create_surface(instance); };
        vkctx_args.surface_size_retriever = [&] { return _window->get_pixel_size(); };
        vkctx_args.validation_layers = is_debug_build();

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

        engine::scene_args scene_args;
        scene_args.name = "New scene";
        scene_args.loaders = _project->get_loader_funcs();
        scene_args.prenderer = _renderer.get();
        _scene = std::make_shared<engine::scene>(scene_args);

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
        vulkan_init_info.PipelineRenderingCreateInfo.depthAttachmentFormat =
            static_cast<VkFormat>(_renderer->depthstencil_attachment().format());
        vulkan_init_info.PipelineRenderingCreateInfo.stencilAttachmentFormat =
            static_cast<VkFormat>(_renderer->depthstencil_attachment().format());

        ImGui_ImplVulkan_Init(&vulkan_init_info);

        const auto scale = SDL_GetWindowDisplayScale(_window->get_handle());

        ImGui::StyleColorsDark();
        ImGui::GetStyle().FontScaleDpi = scale;
        ImGui::GetStyle().FontScaleMain = 0.65F;

        auto& io = ImGui::GetIO();
        io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;
    }

    void editor_window::init_new_scene_dialog(const std::vector<std::string>& available_scenes)
    {
        _new_scene_dialog = std::make_shared<text_input_dialog>(
            "New scene",
            "Name",
            false,
            std::unordered_set<std::string>{ available_scenes.begin(), available_scenes.end() });
        _new_scene_dialog->callbacks().selected = [this](const std::string& selected) {
            engine::scene_args args;
            args.loaders = _project->get_loader_funcs();
            args.name = selected;
            args.prenderer = _renderer.get();

            _scene = std::make_unique<engine::scene>(std::move(args));
        };
        _widget_registry.add_widget(_new_scene_dialog);
    }

    void editor_window::init_open_scene_dialog()
    {
        _open_scene_dialog = std::make_shared<open_scene_dialog>(*_project);
        _open_scene_dialog->callbacks().selected = [this](const std::string& selected) {
            _scene = std::make_shared<engine::scene>(_project->load_scene(selected, _renderer.get()));
        };
        _widget_registry.add_widget(_open_scene_dialog);
    }

    void editor_window::init_save_as_scene_dialog()
    {
        const auto available_scenes = _project->available_scenes();
        const std::unordered_set scene_names(available_scenes.begin(), available_scenes.end());

        _save_scene_as_dialog = std::make_shared<text_input_dialog>("Save scene as", "Name", false, scene_names);
        _save_scene_as_dialog->callbacks().selected = [this](const std::string& selected) {
            _project->save_scene(*_scene, selected);
            _scene = std::make_shared<engine::scene>(_project->load_scene(selected, _renderer.get()));
        };
        _widget_registry.add_widget(_save_scene_as_dialog);
    }

    void editor_window::init_material_manager()
    {
        _material_manager = std::make_shared<material_manager>(*_project);
        _widget_registry.add_widget(_material_manager);

        _material_manager->callbacks().material_renamed = [this](const std::string& old_name, const std::string& new_name) {
            const auto reload_material_in_nodes = [](auto&& nodes, const std::string& old_name, const std::string& new_name) {
                for (const auto& node : nodes)
                {
                    if (node->material_name() == old_name)
                    {
                        node->set_material(new_name);
                    }
                }
            };

            // Delete old material from renderer
            _renderer->vkctx().device().waitIdle();
            _renderer->materials().erase(old_name);

            // Modify unloaded scene node's materials
            for (const auto& scene_name : _project->available_scenes())
            {
                auto nodes = _project->get_scene_nodes(scene_name);
                reload_material_in_nodes(
                    engine::flatten_node_tree(nodes) | engine::filter_nodes<engine::mesh3d_node>(),
                    old_name,
                    new_name);
                _project->replace_scene_nodes(scene_name, nodes);
            }

            // Reload current scene node's materials
            auto scene_nodes = _scene->get_nodes_by_type(engine::node_type::MESH3D_NODE) |
                               engine::filter_nodes<engine::mesh3d_node>() | std::ranges::to<std::vector>();
            reload_material_in_nodes(scene_nodes, old_name, new_name);
        };

        _material_manager->callbacks().material_modified = [this](const std::string& material_name) {
            const auto reload_material_in_nodes = [](auto&& nodes, const std::string& name) {
                for (const auto& node : nodes)
                {
                    if (node->material_name() == name)
                    {
                        node->set_material(std::nullopt);
                        node->set_material(name);
                    }
                }
            };

            // Delete material from renderer to force reload
            _renderer->vkctx().device().waitIdle();
            _renderer->materials().erase(material_name);

            // Modify unloaded scene node's materials
            for (const auto& scene_name : _project->available_scenes())
            {
                auto nodes = _project->get_scene_nodes(scene_name);
                reload_material_in_nodes(
                    engine::flatten_node_tree(nodes) | engine::filter_nodes<engine::mesh3d_node>(),
                    material_name);
                _project->replace_scene_nodes(scene_name, nodes);
            }

            // Reload current scene node's materials
            auto scene_nodes = _scene->get_nodes_by_type(engine::node_type::MESH3D_NODE) |
                               engine::filter_nodes<engine::mesh3d_node>() | std::ranges::to<std::vector>();
            reload_material_in_nodes(scene_nodes, material_name);
        };

        _material_manager->callbacks().material_deleted = [this](const std::string& material_name) {
            _renderer->materials().erase(material_name);
        };
    }

    void editor_window::handle_new_project()
    {
        const std::optional<std::string> dir_open_result = directory_select_dialog();
        if (dir_open_result.has_value())
        {
            const auto& dir = *dir_open_result;
            if (std::filesystem::exists(std::filesystem::path(dir) / ".cathedral"))
            {
                _error_dialog->open("Error", "A project exists within the selected folder already!");
                return;
            }

            _project = std::make_unique<project::project>(project::project::create(dir));
            _project->set_scene_load_callback([this](engine::scene& scene) {
                scene.set_keyboard_input_interface(_keyboard_input);
                scene.set_mouse_input_interface(_mouse_input);
            });

            engine::scene_args scene_args;
            scene_args.loaders = _project->get_loader_funcs();
            scene_args.name = "New scene";
            scene_args.prenderer = _renderer.get();

            _scene = std::make_shared<engine::scene>(std::move(scene_args));
        }
    }

    void editor_window::handle_open_project()
    {
        const std::optional<std::string> dir_open_result = directory_select_dialog();
        if (dir_open_result.has_value())
        {
            switch (const auto load_result = _project->load_project(*dir_open_result))
            {
            case project::load_project_status::OK: {
                engine::scene_args args;
                args.loaders = _project->get_loader_funcs();
                args.name = "New scene";
                args.prenderer = _renderer.get();
                _scene = std::make_shared<engine::scene>(args);
                break;
            }
            case project::load_project_status::PROJECT_PATH_NOT_FOUND:
            case project::load_project_status::PROJECT_FILE_NOT_FOUND:
            case project::load_project_status::PROJECT_FILE_READ_FAILURE:
                _error_dialog->open("Error", std::format("Project load error: {}", magic_enum::enum_name(load_result)));
                break;
            default:
                CRITICAL_ERROR("Unhandled project load status");
            }
        }
    }

    void editor_window::handle_scene_save()
    {
        const auto scenes = _project->available_scenes();
        const auto it = std::ranges::find(scenes, _scene->name());
        if (it != scenes.end())
        {
            _project->save_scene(*_scene, _scene->name());
        }
        else
        {
            handle_scene_save_as();
        }
    }

    void editor_window::handle_scene_save_as()
    {
        _save_scene_as_dialog->open();
    }

    void editor_window::init_ui()
    {
        _error_dialog = std::make_shared<error_dialog>();
        _widget_registry.add_widget(_error_dialog);

        _menubar = std::make_shared<menubar>();

        _menubar->callbacks().new_project = [this] { handle_new_project(); };
        _menubar->callbacks().open_project = [this] { handle_open_project(); };
        _menubar->callbacks().close = [this] { _should_close = true; };

        _menubar->callbacks().new_scene = [this] { _new_scene_dialog->open(); };
        _menubar->callbacks().open_scene = [this] { _open_scene_dialog->open(); };
        _menubar->callbacks().save_scene = [this] { handle_scene_save(); };
        _menubar->callbacks().save_scene_as = [this] { handle_scene_save_as(); };

        _menubar->callbacks().material_manager = [this] { _material_manager->open(); };

        _widget_registry.add_widget(_menubar);

        const auto available_scenes = _project->available_scenes();
        init_new_scene_dialog(available_scenes);
        init_open_scene_dialog();
        init_save_as_scene_dialog();
        init_material_manager();
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
            case SDL_EVENT_WINDOW_RESIZED:
                _renderer->recreate_swapchain_dependent_resources();
                break;
            case SDL_EVENT_WINDOW_CLOSE_REQUESTED:
                _should_close = true;
                break;
            default:
                break;
            }
        }

        // Scene shared_ptr is explicitly copied since the editor window scene might change during the current tick
        const auto scene = _scene;
        scene->tick([&]([[maybe_unused]] const double deltatime) {
            ImGui_ImplVulkan_NewFrame();
            ImGui_ImplSDL3_NewFrame();
            ImGui::NewFrame();

            _widget_registry.tick();

            ImGui::Render();
            ImGui_ImplVulkan_RenderDrawData(
                ImGui::GetDrawData(),
                _renderer->render_cmdbuff(engine::render_cmdbuff_type::OVERLAY));

            if (ImGui::GetIO().ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
            {
                ImGui::UpdatePlatformWindows();
                ImGui::RenderPlatformWindowsDefault();
            }
        });

        // Recalculate viewport for next frame
        std::pair<glm::ivec2, glm::ivec2> viewport = { { 0, 0 }, _renderer->vkctx().get_surface_size() };
        viewport.first.y += static_cast<int>(_menubar->vertical_size());
        _renderer->set_custom_viewport(viewport);

        // Set window title
        SDL_SetWindowTitle(_window->get_handle(), _scene->name().c_str());
    }

    bool editor_window::should_close() const
    {
        return _should_close;
    }
} // namespace cathedral::editor2