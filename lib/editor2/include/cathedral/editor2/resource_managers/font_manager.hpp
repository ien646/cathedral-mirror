#pragma once

#include <cathedral/core.hpp>

#include <cathedral/editor2/dialogs/confirm_dialog.hpp>
#include <cathedral/editor2/dialogs/text_input_dialog.hpp>
#include <cathedral/editor2/engine_window.hpp>
#include <cathedral/editor2/resource_managers/add_font_dialog.hpp>

FORWARD_CLASS(cathedral::engine, scene);
FORWARD_CLASS(cathedral::project, project);

namespace cathedral::editor2
{
    class font_manager
    {
    public:
        explicit font_manager(project::project& pro);

        ~font_manager();

        void execute();

    private:
        engine_window _window;
        project::project& _project;
        std::unique_ptr<engine::scene> _scene;

        one_time_flag _first_tick{ true };

        add_font_dialog _add_font_dialog;
        text_input_dialog _rename_dialog{ "Rename font", "Name" };
        confirm_dialog _delete_confirm_dialog {"Delete font", "Placeholder"};

        std::string _filter;
        std::vector<std::string> _available_font_names;
        std::string _selected_font;

        std::unordered_map<std::string, void*> _texture_ids;

        void init_scene();
        void tick_gui();
    };
} // namespace cathedral::editor2