#include <cathedral/editor2/editor_window/editor_window.hpp>

#include <cathedral/bits/scratch_memory.hpp>
#include <cathedral/engine/scene.hpp>
#include <cathedral/project/project.hpp>

namespace cathedral::editor2
{
    editor_window::editor_window(std::shared_ptr<project::project> project)
        : _project(std::move(project))
    {
        const auto project_path = std::filesystem::path(_project->root_path()).filename();
        _window = std::make_unique<engine_window>(project_path, 800, 600, _project->get_settings());

        engine::scene_args scene_args;
        scene_args.name = "test";
        scene_args.loaders = _project->get_loader_funcs();
        scene_args.prenderer = &_window->renderer();

        _scene = std::make_shared<engine::scene>(scene_args);

        auto node0 = _scene->add_root_node<engine::node>("node0");
        node0->add_child_node<engine::node>("child0");
        node0->add_child_node<engine::node>("child1");
        auto node1 = _scene->add_root_node<engine::node>("node1");
        node1->add_child_node<engine::node>("child2");
        node1->add_child_node<engine::node>("child3");
    }

    int editor_window::execute()
    {
        init_scratch_memory();
        while (_window->keep_open())
        {
            _window->tick([this] {
                _menubar.tick();
                _scene_tree.tick(*_scene);
            });
            flush_scratch_memory();
        }
        return 0;
    }
} // namespace cathedral::editor2