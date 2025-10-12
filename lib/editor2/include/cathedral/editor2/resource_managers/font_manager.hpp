#pragma once

#include <cathedral/core.hpp>

#include <cathedral/editor2/engine_window.hpp>

FORWARD_CLASS(cathedral::engine, scene);
FORWARD_CLASS(cathedral::project, project);

namespace cathedral::editor2
{
    class font_manager
    {
    public:
        explicit font_manager(project::project& pro);

        void execute();

    private:
        engine_window _window;
        project::project& _project;
        std::unique_ptr<engine::scene> _scene;

        bool _first_tick = true;

        std::string _filter;
        std::vector<std::string> _available_font_names;
        std::string _selected_font;

        std::unordered_map<std::string, void*> _texture_ids;

        void init_scene();
        void tick_gui();
    };
} // namespace cathedral::editor2