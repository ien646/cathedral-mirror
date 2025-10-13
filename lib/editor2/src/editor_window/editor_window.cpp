#include <cathedral/editor2/editor_window/editor_window.hpp>

#include <cathedral/bits/scratch_memory.hpp>
#include <cathedral/editor2/native/file_dialog.hpp>
#include <cathedral/editor2/resource_managers/font_manager.hpp>
#include <cathedral/engine/nodes/mesh3d_node.hpp>
#include <cathedral/engine/scene.hpp>
#include <cathedral/project/project.hpp>

namespace cathedral::editor2
{
    editor_window::editor_window(std::shared_ptr<project::project> project)
        : _project(std::move(project))
    {
        const auto project_path = std::filesystem::path(_project->root_path()).filename();
        _window = std::make_unique<engine_window>(project_path, 1200, 800, _project->get_settings());

        engine::scene_args scene_args;
        scene_args.name = "test";
        scene_args.loaders = _project->get_loader_funcs();
        scene_args.prenderer = &_window->renderer();

        _scene = _project->load_scene("perf", &_window->renderer());

        init_inputs();
        init_menubar_callbacks();
    }

    int editor_window::execute()
    {
        size_t scratch_usage = 0;
        while (_window->keep_open())
        {
            for (const auto& pre_tick_callback : _pre_tick_callbacks)
            {
                pre_tick_callback();
            }
            _pre_tick_callbacks.clear();

            _scene->tick([this, scratch_usage](const double deltatime) {
                _window->tick([this, deltatime, scratch_usage] {
                    if (_skip_gui_flag)
                    {
                        _skip_gui_flag = false;
                        return;
                    }

                    const auto dockspace_id = ImGui::DockSpaceOverViewport(
                        0,
                        ImGui::GetMainViewport(),
                        ImGuiDockNodeFlags_PassthruCentralNode | ImGuiDockNodeFlags_NoDockingOverCentralNode);
                    _menubar.tick(*_project->get_settings());
                    _scene_tree.tick(*_scene);
                    _node_properties.tick(_scene_tree.selected_nodes());
                    _viewport.tick(dockspace_id);

                    _input_dialogs.new_scene.tick();
                    _input_dialogs.save_as_scene.tick();
                    _confirm_dialog.tick();
                    _message_dialog.tick();
                    _logs_panel.tick();
                    _stats_panel.tick(*_scene, { { "Scratch buffer usage", std::to_string(scratch_usage) } });
                    _scene_selector_dialog.tick(*_project);
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

            scratch_usage = scratch_memory_usage();
            flush_scratch_memory();
        }
        return 0;
    }

    void editor_window::enqueue_pre_tick_action(std::function<void()> pre_tick_callback)
    {
        _pre_tick_callbacks.push_back(std::move(pre_tick_callback));
    }

    void editor_window::enqueue_post_tick_action(std::function<void()> post_tick_callback)
    {
        _post_tick_callbacks.push_back(std::move(post_tick_callback));
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

    void editor_window::init_menubar_callbacks()
    {
        _menubar.callbacks.close = [this] { _window->close(); };

        _menubar.callbacks.fonts = [this] {
            const auto saved_context = _window->get_imgui_context();
            font_manager(*_project).execute();
            ImGui::SetCurrentContext(saved_context);
        };

        _menubar.callbacks.materials = [this] {};

        _menubar.callbacks.meshes = [this] {};

        _menubar.callbacks.new_project = [this] {};

        _menubar.callbacks.new_scene = [this] {
            _input_dialogs.new_scene.set_text("new scene");
            _input_dialogs.new_scene.open();
        };

        _menubar.callbacks.open_project = [this] {
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
        };

        _menubar.callbacks.open_scene = [this] {
            _scene_selector_dialog.callbacks.selected = [this](const std::string& selected_scene) {
                enqueue_pre_tick_action([=, this] { _scene = _project->load_scene(selected_scene, &_window->renderer()); });
            };
            _scene_selector_dialog.open();
        };

        _menubar.callbacks.save_as_scene = [this] {
            _input_dialogs.save_as_scene.set_text(_scene->name());
            _input_dialogs.save_as_scene.open();
        };

        _menubar.callbacks.save_scene = [this] {
            if (std::ranges::contains(_project->available_scenes(), _scene->name()))
            {
                _project->save_scene(*_scene, _scene->name());
            }
            else
            {
                _menubar.callbacks.save_as_scene();
            }
        };

        _menubar.callbacks.shaders = [this] {};

        _menubar.callbacks.textures = [this] {};

        _menubar.callbacks.capture_screenshot = [this] {
            const std::chrono::year_month_day now_date(
                std::chrono::floor<std::chrono::days>(std::chrono::system_clock::now()));
            const auto target_file = native_save_file(std::format("Screenshot_{}.png", now_date));

            if (target_file.has_value())
            {
                enqueue_pre_tick_action([this, file = *target_file] {
                    hide_ui_for_this_frame();
                    enqueue_post_tick_action(
                        [this, file] { _scene->get_renderer().capture_screenshot().write_to_file_png(file); });
                });
            }
        };

        _menubar.callbacks.settings_changed = [this] { _project->save_settings(); };
    }
} // namespace cathedral::editor2