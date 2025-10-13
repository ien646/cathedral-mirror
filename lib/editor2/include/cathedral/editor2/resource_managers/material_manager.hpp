#pragma once

#include <cathedral/core.hpp>

#include <cathedral/editor2/dialogs/confirm_dialog.hpp>
#include <cathedral/editor2/dialogs/text_input_dialog.hpp>
#include <cathedral/editor2/engine_window.hpp>

FORWARD_CLASS(cathedral::engine, scene);
FORWARD_CLASS(cathedral::project, project);

namespace cathedral::editor2
{
    class material_manager
    {
    public:
        explicit material_manager(project::project& pro);

        void execute();

    private:
        engine_window _window;
        project::project& _project;
        std::unique_ptr<engine::scene> _scene;

        one_time_flag _first_tick{ true };

        text_input_dialog _rename_dialog{ "Rename material", "Name" };
        confirm_dialog _delete_confirm_dialog {"Delete material", "Placeholder"};

        std::string _filter;
        std::vector<std::string> _available_material_names;
        std::vector<std::string> _available_vertex_shaders;
        std::vector<std::string> _available_fragment_shaders;
        std::string _selected_material;

        void init_scene();
        void init_shaders();

        void tick_gui();
        void tick_properties();
    };
} // namespace cathedral::editor2