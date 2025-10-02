#include "imgui_internal.h"

#include <cathedral/editor2/editor_window/editor_window.hpp>

#include <cathedral/bits/scratch_memory.hpp>
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

        _scene = std::make_shared<engine::scene>(scene_args);

        auto monki = _scene->add_root_node<engine::mesh3d_node>("monki");
        monki->set_material("monki");
        monki->set_mesh("monki");
    }

    int editor_window::execute()
    {
        while (_window->keep_open())
        {
            _scene->tick([this](const double deltatime) {
                _window->tick([this] {
                    const auto dockspace_id = ImGui::DockSpaceOverViewport(
                        0,
                        ImGui::GetMainViewport(),
                        ImGuiDockNodeFlags_PassthruCentralNode | ImGuiDockNodeFlags_NoDockingOverCentralNode);
                    _menubar.tick();
                    _scene_tree.tick(*_scene);
                    _viewport.tick(dockspace_id);
                });

                _scene->get_renderer().set_custom_viewport(
                    std::make_pair(_viewport.position(), _viewport.size() + _viewport.position()));
            });

            flush_scratch_memory();
        }
        return 0;
    }
} // namespace cathedral::editor2