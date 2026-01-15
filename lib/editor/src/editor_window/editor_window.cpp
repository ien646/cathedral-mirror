#include <cathedral/editor/editor_window/editor_window.hpp>

#include <cathedral/bits/scratch_memory.hpp>
#include <cathedral/editor/native/file_dialog.hpp>
#include <cathedral/editor/resource_managers/font_manager.hpp>
#include <cathedral/editor/resource_managers/material_manager.hpp>
#include <cathedral/engine/nodes/mesh3d_node.hpp>
#include <cathedral/engine/scene.hpp>
#include <cathedral/project/project.hpp>
#include <cathedral/sdl/event.hpp>

#include <imgui_internal.h>

namespace cathedral::editor
{
    editor_window::editor_window(std::shared_ptr<project::project> project)
        : event_bus_subscriber(get_event_bus(register_event_bus("editor_event_bus")))
        , _menubar(_event_bus)
        , _node_properties(*project)
        , _project(MOVE(project))
    {
        const auto project_path = std::filesystem::path(_project->root_path()).filename().string();
        _window = std::make_unique<engine_window>(project_path, 1200, 800, _project->get_settings());

        engine::scene_args scene_args;
        scene_args.name = "New scene";
        scene_args.loaders = _project->get_loader_funcs();
        scene_args.prenderer = &_window->renderer();

        _scene = std::make_unique<engine::scene>(scene_args);

        init_inputs();
        init_subscriptions();
    }

    int editor_window::execute()
    {
        size_t scratch_usage = 0;
        while (_window->keep_open())
        {
            sdl::global_poll_events();
            for (const auto& pre_tick_callback : _pre_tick_callbacks)
            {
                pre_tick_callback();
            }
            _pre_tick_callbacks.clear();

            _scene->tick([this, scratch_usage]([[maybe_unused]] const double deltatime) {
                _window->tick([this, scratch_usage] {
                    if (_skip_gui_flag)
                    {
                        _skip_gui_flag = false;
                        return;
                    }

                    auto dockspace_id = ImGui::DockSpaceOverViewport(
                        0,
                        ImGui::GetMainViewport(),
                        ImGuiDockNodeFlags_PassthruCentralNode | ImGuiDockNodeFlags_NoDockingOverCentralNode);

                    if (!_window->editor_settings()->get(editor_setting::EDITOR_WINDOW_SETUP_COMPLETE).as_bool())
                    {
                        ImGuiID dock_left, dock_right, dock_bottom, dock_bottom_left, dock_bottom_right;
                        ImGui::DockBuilderSplitNode(dockspace_id, ImGuiDir_Down, 0.25F, &dock_bottom, &dockspace_id);
                        ImGui::DockBuilderSplitNode(dockspace_id, ImGuiDir_Left, 0.25F, &dock_left, &dockspace_id);
                        ImGui::DockBuilderSplitNode(dockspace_id, ImGuiDir_Right, 0.25F / 0.75F, &dock_right, &dockspace_id);
                        ImGui::DockBuilderSplitNode(dock_bottom, ImGuiDir_Left, 0.5F, &dock_bottom_left, &dock_bottom_right);

                        ImGui::DockBuilderDockWindow(scene_tree::WINDOW_ID, dock_left);
                        ImGui::DockBuilderDockWindow(node_properties::WINDOW_ID, dock_right);
                        ImGui::DockBuilderDockWindow(logs_panel::WINDOW_ID, dock_bottom_left);
                        ImGui::DockBuilderDockWindow(stats_panel::WINDOW_ID, dock_bottom_right);

                        ImGui::DockBuilderFinish(dockspace_id);

                        _window->editor_settings()->set(editor_setting::EDITOR_WINDOW_SETUP_COMPLETE, true);
                        _project->save_settings();
                    }

                    _menubar.tick();
                    _scene_tree.tick(*_scene);
                    _node_properties.tick(*_scene, _scene_tree.selected_nodes());
                    _viewport.tick(dockspace_id);

                    _input_dialogs.new_scene.tick();
                    _input_dialogs.save_as_scene.tick();
                    _confirm_dialog.tick();
                    _message_dialog.tick();
                    _logs_panel.tick();
                    _stats_panel.tick(*_scene, { { "Scratch buffer usage", std::to_string(scratch_usage) } });
                    _scene_selector_dialog.tick(*_project);
                    _settings_dialog.tick(*_project, *_window->editor_settings());
                });

                const auto scale = _window->window().get_scale();
                const auto vp_pos = glm::vec2(_viewport.position()) * scale;
                const auto vp_size = glm::vec2(_viewport.size()) * scale;

                _scene->get_renderer().set_custom_viewport(std::make_pair<glm::ivec2, glm::ivec2>(vp_pos, vp_size + vp_pos));
            });

            for (const auto& post_tick_callback : _post_tick_callbacks)
            {
                post_tick_callback();
            }
            _post_tick_callbacks.clear();

            const auto tick_manager = [](auto&& manager) {
                if (!manager)
                {
                    return;
                }
                if (manager->must_close())
                {
                    manager.reset();
                }
                else
                {
                    manager->tick();
                }
            };

            tick_manager(_font_manager);
            tick_manager(_material_manager);
            tick_manager(_mesh_manager);
            tick_manager(_script_manager);
            tick_manager(_shader_manager);
            tick_manager(_texture_manager);

            scratch_usage = scratch_memory_usage();
            flush_scratch_memory();
        }
        return 0;
    }

    void editor_window::enqueue_pre_tick_action(std::function<void()> pre_tick_callback)
    {
        _pre_tick_callbacks.push_back(MOVE(pre_tick_callback));
    }

    void editor_window::enqueue_post_tick_action(std::function<void()> post_tick_callback)
    {
        _post_tick_callbacks.push_back(MOVE(post_tick_callback));
    }

    void editor_window::hide_ui_for_this_frame()
    {
        _skip_gui_flag = true;
    }

    void editor_window::init_inputs()
    {
        const auto new_scene_name_validator = [this](const std::string& text) -> bool {
            return !text.empty() && std::ranges::all_of(_project->available_scenes(), [&](const std::string& scene_name) {
                return text != scene_name;
            });
        };

        // New scene
        _input_dialogs.new_scene.callbacks.accepted = [this] {
            const auto name = _input_dialogs.new_scene.text();

            engine::scene_args args;
            args.name = name;
            args.loaders = _project->get_loader_funcs();
            args.prenderer = &_window->renderer();

            enqueue_pre_tick_action([=, this] { _scene = std::make_shared<engine::scene>(args); });
        };
        _input_dialogs.new_scene.set_validator(new_scene_name_validator);

        // Save as scene
        _input_dialogs.save_as_scene.callbacks.accepted = [this] {
            const auto name = _input_dialogs.save_as_scene.text();
            _confirm_dialog.set_label(std::format("Scene with name '{}' already exists.\nOverwrite?", name));
            _confirm_dialog.callbacks.accepted = [=, this] { _project->save_scene(*_scene, name); };
            _confirm_dialog.open();
        };
    }

    void editor_window::init_subscriptions()
    {
        subscribe<events::font_manager_open>([this](const auto&) { open_font_manager(); });

        subscribe<events::material_manager_open>([this](const auto&) { open_material_manager(); });

        subscribe<events::mesh_manager_open>([this](const auto&) { open_mesh_manager(); });

        subscribe<events::script_manager_open>([this](const auto&) { open_script_manager(); });

        subscribe<events::shader_manager_open>([this](const auto&) { open_shader_manager(); });

        subscribe<events::texture_manager_open>([this](const auto&) { open_texture_manager(); });

        subscribe<events::new_project>([](const auto&) { NOT_IMPLEMENTED(); });

        subscribe<events::open_project>([this](const auto&) { open_project(); });

        subscribe<events::new_scene>([this](const auto&) { new_scene(); });

        subscribe<events::open_scene>([this](const auto&) { open_scene(); });

        subscribe<events::save_as_scene>([this](const auto&) { save_as_scene(); });

        subscribe<events::save_scene>([this](const auto&) { save_scene(); });

        subscribe<events::capture_screenshot>([this](const auto&) { capture_screenshot(); });

        subscribe<events::editor_window_reset_layout>(
            [this](const auto&) { _window->editor_settings()->set(editor_setting::EDITOR_WINDOW_SETUP_COMPLETE, false); });

        subscribe<events::global_text_scale_down>([this](const auto&) { text_scale_down(); });

        subscribe<events::global_text_scale_up>([this](const auto&) { text_scale_up(); });

        subscribe<events::settings_dialog_open>([this](const auto&) { _settings_dialog.open(); });
    }

    void editor_window::open_project()
    {
        const auto open_dir_result = native_open_dir();
        if (open_dir_result.has_value())
        {
            switch (_project->load_project(open_dir_result.value()))
            {
            case project::load_project_status::OK:
                break;
            case project::load_project_status::PROJECT_PATH_NOT_FOUND:
                _message_dialog.set_mode(message_dialog_mode::ERROR);
                _message_dialog.set_title("Project load error");
                _message_dialog.set_text("Project path not found");
                _message_dialog.open();
                break;
            case project::load_project_status::PROJECT_FILE_NOT_FOUND:
                _message_dialog.set_mode(message_dialog_mode::ERROR);
                _message_dialog.set_title("Project load error");
                _message_dialog.set_text("Project file not found");
                _message_dialog.open();
                break;
            case project::load_project_status::PROJECT_FILE_READ_FAILURE:
                _message_dialog.set_mode(message_dialog_mode::ERROR);
                _message_dialog.set_title("Project load error");
                _message_dialog.set_text("Project file read failure");
                _message_dialog.open();
                break;
            }
        }
    }

    void editor_window::open_font_manager()
    {
        auto* const saved_context = _window->get_imgui_context();
        _font_manager = std::make_unique<font_manager>(*_project, *_window->editor_settings(), _event_bus);
        ImGui::SetCurrentContext(saved_context);
    }

    void editor_window::open_material_manager()
    {
        auto* const saved_context = _window->get_imgui_context();
        _material_manager = std::make_unique<material_manager>(*_project, *_window->editor_settings(), _event_bus);
        ImGui::SetCurrentContext(saved_context);
    }

    void editor_window::open_mesh_manager()
    {
        auto* const saved_context = _window->get_imgui_context();
        _mesh_manager = std::make_unique<mesh_manager>(*_project, *_window->editor_settings());
        ImGui::SetCurrentContext(saved_context);
    }

    void editor_window::open_script_manager()
    {
        auto* const saved_context = _window->get_imgui_context();
        _script_manager = std::make_unique<script_manager>(*_project, *_window->editor_settings());
        ImGui::SetCurrentContext(saved_context);
    }

    void editor_window::open_shader_manager()
    {
        auto* const saved_context = _window->get_imgui_context();
        _shader_manager = std::make_unique<shader_manager>(*_project, *_window->editor_settings());
        ImGui::SetCurrentContext(saved_context);
    }

    void editor_window::open_texture_manager()
    {
        auto* const saved_context = _window->get_imgui_context();
        _texture_manager = std::make_unique<texture_manager>(*_project, *_window->editor_settings());
        ImGui::SetCurrentContext(saved_context);
    }

    void editor_window::new_scene()
    {
        _input_dialogs.new_scene.set_text("new scene");
        _input_dialogs.new_scene.open();
    }

    void editor_window::open_scene()
    {
        _scene_selector_dialog.callbacks.selected = [this](const std::string& selected_scene) {
            enqueue_pre_tick_action([=, this] { _scene = _project->load_scene(selected_scene, &_window->renderer()); });
        };
        _scene_selector_dialog.open();
    }

    void editor_window::save_as_scene()
    {
        _input_dialogs.save_as_scene.set_text(_scene->name());
        _input_dialogs.save_as_scene.open();
    }

    void editor_window::save_scene() const
    {
        if (std::ranges::contains(_project->available_scenes(), _scene->name()))
        {
            _project->save_scene(*_scene, _scene->name());
        }
        else
        {
            _event_bus.publish<events::save_as_scene>();
        }
    }

    void editor_window::capture_screenshot()
    {
        const std::chrono::year_month_day now_date(std::chrono::floor<std::chrono::days>(std::chrono::system_clock::now()));
        const auto target_file = native_save_file(std::format("Screenshot_{}.png", now_date));

        if (target_file.has_value())
        {
            enqueue_pre_tick_action([this, file = *target_file] {
                hide_ui_for_this_frame();
                enqueue_post_tick_action(
                    [this, file] { _scene->get_renderer().capture_screenshot().write_to_file_png(file); });
            });
        }
    }

    void editor_window::text_scale_down() const
    {
        const auto scale = std::max(0.1F, ImGui::GetIO().FontGlobalScale - 0.1F);
        ImGui::GetIO().FontGlobalScale = scale;
        _window->editor_settings()->set(editor_setting::TEXT_SCALE, scale);
        _project->save_settings();
    }

    void editor_window::text_scale_up() const
    {
        const auto scale = ImGui::GetIO().FontGlobalScale + 0.1F;
        ImGui::GetIO().FontGlobalScale = scale;
        _window->editor_settings()->set(editor_setting::TEXT_SCALE, scale);
        _project->save_settings();
    }
} // namespace cathedral::editor